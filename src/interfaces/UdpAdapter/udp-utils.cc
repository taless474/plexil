// -*- Mode: C++ -*-
// File: plexil/src/interfaces/UdpAdapter/udp-utils.cc
// Date: Thu Jul  7 16:56:50 2011

//#include <stdio.h>
//#include <iostream>           // cout
//#include <math.h>             // pow()

#include "udp-utils.hh"

namespace PLEXIL
{
  //
  // 32 bit versions of float and int conversions
  //

  long int float_to_long_int (float num)
  {
    //printf("\n\nsizeof(long int) = %d, sizeof(float) = %d\n\n", sizeof(long int), sizeof(float));
    float x = num;
    void* y = &x;
    return *(long int *) y;
  }

  float long_int_to_float (long int num)
  {
    long int x = num;
    void* y = &x;
    return *(float *) y;
  }

  int network_bytes_to_number(unsigned char* buffer, int start_index, int total_bits, bool is_signed=true, bool debug=false)
  {
    int value = 0;
    int i = total_bits - 8;
    int cursor = start_index;
    for (i ; i >= 0 ; i -= 8)
      {
        if (debug) printf("buffer[%d]==%d; shift>> %d bits; ", cursor, buffer[cursor], i);
        value += (buffer[cursor++] & 0xff) << i;
        if (debug) std::cout << "; value=" << value << std::endl;
      }
    if (is_signed && (value >= pow(2.0,total_bits)/2.0))
      {
        value -= pow(2.0, total_bits);
      }
    return value;
  }

  void number_to_network_bytes(int number, unsigned char* buffer, int start_index, int total_bits, bool debug=false)
  {
    int i = total_bits - 8;
    int cursor = start_index;
    for (i ; i >= 0 ; i -= 8)
      {
        if (debug) std::cout << "number=" << number << ": shift>> " << i << " bits; ";
        buffer[cursor++] = (int)((number >> i) & 0xff);
        if (debug) printf("buffer[%d]==%d\n", cursor, buffer[cursor - 1]);
      }
  }

  void encode_long_int(long int long_int, unsigned char* buffer, int start_index)
  // Encode a 32 bit integer (in network byte order)
  {
    number_to_network_bytes(htonl(long_int), buffer, start_index, 32, false);
  }

  long int decode_long_int(unsigned char* buffer, int start_index)
  // Decode a 32 bit integer from the network bytes in host byte order
  {
    ntohl(network_bytes_to_number(buffer, 0, 32, false, false));
  }

  void encode_float(float num, unsigned char* buffer, int start_index)
  // Encode a 32 bit float in network byte order
  {
    long int temp = htonl(float_to_long_int(num));
    number_to_network_bytes(temp, buffer, start_index, 32, false);
  }

  float decode_float(unsigned char* buffer, int start_index)
  // Decode a 32 bit float from network byte order
  {
    // ntohl called in decode_long_int
    long int temp = decode_long_int(buffer, start_index);
    return long_int_to_float(temp);
  }

  // void reverse_bytes(unsigned char* buffer, int start_index, int num_bytes, bool debug=false)
  // // Reverse the bytes in the buffer from start_index for num_bytes
  // {
  //   unsigned char* temp = new unsigned char[num_bytes];
  //   int cursor = 0;
  //   // Copy the bytes into a temp buffer
  //   for (int i = start_index ; i < start_index + num_bytes ; i++)
  //     {
  //       if (debug) printf("cursor=%d, i=%d, temp[%d]=%d, buffer[%d]=%d\n", cursor, i, cursor, temp[cursor], i, buffer[i]);
  //       temp[cursor++] = buffer[i];
  //     }
  //   // Copy them back in the reverse order
  //   for (int i = start_index ; i < start_index + num_bytes ; i++)
  //     {
  //       buffer[i] = temp[--cursor];
  //       if (debug) printf("cursor=%d, i=%d, temp[%d]=%d, buffer[%d]=%d\n", cursor, i, cursor, temp[cursor], i, buffer[i]);
  //     }
  //   delete temp;
  // }
}

// EOF
