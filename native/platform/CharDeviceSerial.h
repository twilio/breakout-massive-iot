#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>

#include <stdexcept>

class CharDeviceSerial : public IOwlSerial {
 public:
  CharDeviceSerial(const char *device_path) {
    fd = open(device_path, O_RDWR | O_NONBLOCK);
    if (fd < 0) {
      throw std::runtime_error("Cannot open serial device");
    }

    struct termios options;
    tcgetattr(fd, &options);

    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);

    options.c_cflag |= (CLOCAL | CREAD);
    options.c_iflag &= ~(INLCR | IGNCR | ICRNL);
    options.c_oflag &= ~(ONLCR | OCRNL | ONOCR | ONLRET | OLCUC | OPOST);
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    tcsetattr(fd, TCSANOW, &options);
  }

  virtual ~CharDeviceSerial() {
    close(fd);
  }

  int32_t available() {
    int len;
    ioctl(fd, FIONREAD, &len);
    return len;
  }

  int32_t read(uint8_t *buf, uint32_t count) {
    ssize_t res = ::read(fd, buf, count);  // Standard library call
    fprintf(stderr, "READ: %.*s\n", res, buf);
    return res;
  }

  int32_t write(const uint8_t *buf, uint32_t count) {
    ssize_t res = ::write(fd, buf, count);  // Standard library call
    fprintf(stderr, "WRITE: %.*s\n", res, buf);
    return res;
  }

 private:
  int fd;
};
