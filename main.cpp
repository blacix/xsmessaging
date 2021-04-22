#include <iostream>

#include "App.h"
#include "serializer/serializer.h"
#include "serializer/serializerbuffer.h"
#include "xsmReceiver.h"
#include "xsmSender.h"

int main() {

  // implements sending data over real connection 
  // by hooking sending and receiving together for testing
  Connection connection;

  xsm::Sender sender(xsm::Escaping::ON, connection);

  // a test protocol impl. over stream based connection
  Protocol protocol(sender);

  xsm::Receiver receiver(xsm::Escaping::ON, protocol);

  // for testing: hooking rx and tx
  connection.setReceiver(&receiver);


  // test sending
  uint8_t buffer[xsm::MAX_PAYLOAD_SIZE];

  nowtech::SerializerBuffer serializerBuffer(buffer, xsm::MAX_PAYLOAD_SIZE);
  serializerBuffer.setAvailablaLength(xsm::MAX_PAYLOAD_SIZE);
  nowtech::Serializer serializer(serializerBuffer, 0, 0);

  serializer << "hello world! " << 42 << 42.42F << static_cast<uint8_t>(0b00101111);
  xsm::Message msg;
  msg.Size = serializer.getLength();
  serializer.reset();
  serializer.read(msg.Data.data(), msg.Size);
  sender.send(msg);

  serializer.reset();
  serializer << "another message with string and numbers! " << 3 << 3.14F << 'c';
  msg.Size = serializer.getLength();
  serializer.reset();
  serializer.read(msg.Data.data(), msg.Size);
  sender.send(msg);


  // xsm::Message msg1{{'a', 'b', 'c', 'd'}, 4};
  // msg1.Data[3] = xsm::FRAME_DELIMITER; // test escaping
  // sender.send(msg1);

  // xsm::Message msg2{{'a', 'b', 'c', 'd'}, 4};
  // msg2.Data[3] = xsm::ESCAPE_BYTE; // test escaping
  // sender.send(msg2);


  getchar();
  return 0;
}
