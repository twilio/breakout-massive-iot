#include <histedit.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <atomic>
#include <future>
#include <modem/OwlModemBG96.h>
#include "CharDeviceSerial.h"

#define OVERWRITE_TLS_CREDS false

static void print_message(str topic, str data) {
  fprintf(stderr, "MQTT message: %.*s - %.*s\n", topic.len, topic.s, data.len, data.s);
}

static const char* prompt(EditLine* e) {
  return "owlmqtt> ";
}

static bool map_file(const char* filename, char** buf, int* len) {
  struct stat sb;
  int fd = open(filename, O_RDONLY);

  if (fd < 0) {
    return false;
  }

  fstat(fd, &sb);
  *len = sb.st_size;

  *buf = (char*)mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  close(fd);

  if (*buf == nullptr) {
    return false;
  }
  return true;
}

bool spin_thread(OwlModemBG96& modem, std::atomic_bool& cancel) {
  while (!cancel) {
    usleep(200);
    modem.AT.spin();
  }
  return true;
}

int main(int argc, const char** argv) {
  if (argc != 4 && argc != 7) {
    fprintf(stderr,
            "Usage: mqtt-sample </path/to/device> <cops_numeric> <APN> [</path/to/CA.pem> </path/to/cert.pem> "
            "</path/to/pkey.pem>]");
    return 1;
  }

  const char* device_path = argv[1];
  const char* cops_arg    = argv[2];
  const char* apn         = argv[3];
  const char* ca_path     = nullptr;
  const char* cert_path   = nullptr;
  const char* pkey_path   = nullptr;
  bool use_tls            = false;

  str tls_cert;
  str tls_pkey;
  str tls_cacert;

  if (argc == 7) {
    ca_path   = argv[4];
    cert_path = argv[5];
    pkey_path = argv[6];
    use_tls   = true;

    if (!map_file(ca_path, &tls_cacert.s, &tls_cacert.len)) {
      fprintf(stderr, "Failed to read server CA certificate\n");
      return 1;
    }

    if (!map_file(cert_path, &tls_cert.s, &tls_cert.len)) {
      fprintf(stderr, "Failed to device certificate\n");
      return 1;
    }

    if (!map_file(pkey_path, &tls_pkey.s, &tls_pkey.len)) {
      fprintf(stderr, "Failed to read device private key\n");
      return 1;
    }
  }

  owl_log_set_level(L_DBG);
  CharDeviceSerial serial(device_path);
  OwlModemBG96 bg96(&serial);

  if (!bg96.powerOn()) {
    fprintf(stderr, "Failed to power on the modem\n");
    return 1;
  }

  bool res;
  if (cops_arg) {
    res = bg96.initModem(apn, cops_arg, AT_COPS__Format__Numeric);
  } else {
    res = bg96.initModem(apn);
  }

  if (!res) {
    fprintf(stderr, "Failed to initialize the modem\n");
    return 1;
  }

  fprintf(stderr, "Modem initialized, waiting for network registration\n");

  if (!bg96.waitForNetworkRegistration(30000)) {
    fprintf(stderr, "Failed to register on network\n");
    return 1;
  }

  if (use_tls) {
    if (!bg96.ssl.initContext()) {
      fprintf(stderr, "Failed to initialize TLS context\n");
      return 1;
    }

    if (!bg96.ssl.setDeviceCert(tls_cert, OVERWRITE_TLS_CREDS)) {
      fprintf(stderr, "Failed to set device certificate\n");
      return 1;
    }

    if (!bg96.ssl.setDevicePkey(tls_pkey, OVERWRITE_TLS_CREDS)) {
      fprintf(stderr, "Failed to set device private key\n");
      return 1;
    }

    if (!bg96.ssl.setServerCA(tls_cacert, OVERWRITE_TLS_CREDS)) {
      fprintf(stderr, "Failed to set device private key\n");
      return 1;
    }

    bg96.mqtt.useTLS(use_tls);
  }

  EditLine* el = el_init(argv[0], stdin, stdout, stderr);
  el_set(el, EL_PROMPT, &prompt);
  el_set(el, EL_EDITOR, "emacs");

  History* owl_history = history_init();

  if (!owl_history) {
    fprintf(stderr, "Failed to initialize editline history");
    return 1;
  }

  HistEvent ev;
  history(owl_history, &ev, H_SETSIZE, 800);
  el_set(el, EL_HIST, history, owl_history);

  Tokenizer* owl_tokenizer = tok_init(NULL);
  for (;;) {
    int count;
    std::atomic_bool cancel{false};

    auto spin_async  = std::async(std::launch::async, spin_thread, std::ref(bg96),
                                 std::ref(cancel));  // spin on modem while waiting for user input
    const char* line = el_gets(el, &count);
    cancel           = true;
    spin_async.get();

    if (count <= 0) {
      continue;
    }

    history(owl_history, &ev, H_ENTER, line);

    int tok_argc;
    const char** tok_argv;
    tok_reset(owl_tokenizer);
    tok_str(owl_tokenizer, line, &tok_argc, &tok_argv);

    if (tok_argc < 1) {
      continue;
    }

    if (strcmp(tok_argv[0], "open") == 0) {
      if (tok_argc != 3) {
        fprintf(stderr, "Usage: open <broker_addr> <broker_port>\n");
        continue;
      }

      unsigned long int port = strtoul(tok_argv[2], NULL, 10);
      if (!bg96.mqtt.openConnection(tok_argv[1], port)) {
        fprintf(stderr, "Failed to open connection to MQTT broker\n");
      }
    } else if (strcmp(tok_argv[0], "login") == 0) {
      if (tok_argc != 2 && tok_argc != 4) {
        fprintf(stderr, "Usage: login <client_id> [<login> <password>]\n");
        continue;
      }

      const char* login    = NULL;
      const char* password = NULL;
      if (tok_argc == 4) {
        login    = tok_argv[2];
        password = tok_argv[3];
      }

      if (!bg96.mqtt.login(tok_argv[1], login, password)) {
        fprintf(stderr, "Failed to login to MQTT broker\n");
      }
    } else if (strcmp(tok_argv[0], "pub") == 0) {
      if (tok_argc != 3) {
        fprintf(stderr, "Usage: pub <topic> <message>\n");
        continue;
      }

      str to_publish = STRDECL(tok_argv[2]);

      if (!bg96.mqtt.publish(tok_argv[1], to_publish)) {
        fprintf(stderr, "Failed to publish to MQTT broker\n");
      }
    } else if (strcmp(tok_argv[0], "sub") == 0) {
      if (tok_argc != 2) {
        fprintf(stderr, "Usage: sub <topic>\n");
        continue;
      }

      bg96.mqtt.setMessageCallback(print_message);

      if (!bg96.mqtt.subscribe(tok_argv[1], 1)) {
        fprintf(stderr, "Failed to subscribe to a topic\n");
      }
    } else if (strcmp(tok_argv[0], "exit") == 0) {
      break;
    }
  }

  history_end(owl_history);
  el_end(el);

  munmap(tls_cacert.s, tls_cacert.len);
  munmap(tls_cert.s, tls_cert.len);
  munmap(tls_pkey.s, tls_pkey.len);

  return 0;
}

// TODO: find a better way to insert a test point
void spinProcessLineTestpoint(str line) {
  return;
}
