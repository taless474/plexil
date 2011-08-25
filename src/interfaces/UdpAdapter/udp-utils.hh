// -*- Mode: C++ -*-
// File: plexil/src/interfaces/UdpAdapter/udp-utils.hh
// Date: Fri Jul 15 13:40:53 2011

#include <stdio.h>
#include <iostream>           // cout
#include <math.h>             // pow()
#include <arpa/inet.h>        // htonl
#include <netdb.h>            // gethostbyname

namespace PLEXIL
{

  struct udp_thread_params
  {
    int local_port;
    unsigned char* buffer;
    size_t size;
    int sock;
    bool debug;
  };

  long int float_to_long_int (float num);
  float long_int_to_float (long int num);
  int network_bytes_to_number(const unsigned char* buffer, int start_index, int total_bits, bool is_signed, bool debug=false);
  void number_to_network_bytes(int number, unsigned char* buffer, int start_index, int total_bits, bool debug=false);
  void encode_long_int(long int num, unsigned char* buffer, int start_index);
  void encode_short_int(long int num, unsigned char* buffer, int start_index);
  long int decode_long_int(const unsigned char* buffer, int start_index);
  short int decode_short_int(const unsigned char* buffer, int start_index);
  void encode_float(float num, unsigned char* buffer, int start_index);
  float decode_float(const unsigned char* buffer, int start_index);
  void encode_string(const std::string str, unsigned char* buffer, int start_index);
  std::string decode_string(const unsigned char* buffer, int start_index, int length);
  int udp_tests(void);
  void print_buffer(const unsigned char* buffer, int bytes, bool fancy=false);

  int send_message_connect(const char* peer_host, int peer_port, const char* buffer, size_t size, bool debug=false);
  int send_message_bind(int local_port, const char* peer_host, int peer_port, const char* buffer, size_t size, bool debug=false);
  int wait_for_input_on_thread(udp_thread_params* params);
  int wait_for_input(int local_port, unsigned char* buffer, size_t size, int sock, bool debug=false);
}

// EOF
