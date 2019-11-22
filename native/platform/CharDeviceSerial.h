#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>

#include <stdexcept>

class CharDeviceSerial : public IOwlSerial {
 public:
  CharDeviceSerial(const char *device_path, int baudrate) {
    fd = open(device_path, O_RDWR | O_NONBLOCK);
    if (fd < 0) {
      throw std::runtime_error("Cannot open serial device");
    }

    struct termios options;
    tcgetattr(fd, &options);

    options.c_cflag = CS8;

    switch (baudrate) {
      case 50:
        options.c_cflag |= B50;
        break;

      case 75:
        options.c_cflag |= B75;
        break;

      case 110:
        options.c_cflag |= B110;
        break;

      case 134:
        options.c_cflag |= B134;
        break;

      case 150:
        options.c_cflag |= B150;
        break;

      case 200:
        options.c_cflag |= B200;
        break;

      case 300:
        options.c_cflag |= B300;
        break;

      case 600:
        options.c_cflag |= B600;
        break;

      case 1200:
        options.c_cflag |= B1200;
        break;

      case 1800:
        options.c_cflag |= B1800;
        break;

      case 2400:
        options.c_cflag |= B2400;
        break;

      case 4800:
        options.c_cflag |= B4800;
        break;

      case 9600:
        options.c_cflag |= B9600;
        break;

      case 19200:
        options.c_cflag |= B19200;
        break;

      case 38400:
        options.c_cflag |= B38400;
        break;

      case 57600:
        options.c_cflag |= B57600;
        break;

      case 115200:
        options.c_cflag |= B115200;
        break;

      case 230400:
        options.c_cflag |= B230400;
        break;

#ifdef B460800
      case 460800:
        options.c_cflag |= B460800;
        break;
#endif

#ifdef B500000
      case 500000:
        options.c_cflag |= B500000;
        break;
#endif

#ifdef B576000
      case 576000:
        options.c_cflag |= B576000;
        break;
#endif

#ifdef B921600
      case 921600:
        options.c_cflag |= B921600;
        break;
#endif

#ifdef B1000000
      case 1000000:
        options.c_cflag |= B1000000;
        break;
#endif

#ifdef B1152000
      case 11520000:
        options.c_cflag |= B1152000;
        break;
#endif

#ifdef B1500000
      case 1500000:
        options.c_cflag |= B1500000;
        break;
#endif

#ifdef B2000000
      case 2000000:
        options.c_cflag |= B2000000;
        break;
#endif

#ifdef B2500000
      case 2500000:
        options.c_cflag |= B2500000;
        break;
#endif

#ifdef B3000000
      case 3000000:
        options.c_cflag |= B3000000;
        break;
#endif

#ifdef B3500000
      case 3500000:
        options.c_cflag |= B3500000;
        break;
#endif

#ifdef B4000000
      case 4000000:
        options.c_cflag |= B4000000;
        break;
#endif

      default:
        close(fd);
        throw std::runtime_error("Invalid baud rate");
    }

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
    return res;
  }

  int32_t write(const uint8_t *buf, uint32_t count) {
    ssize_t res = ::write(fd, buf, count);  // Standard library call
    return res;
  }

 private:
  int fd;
};
