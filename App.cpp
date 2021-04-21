#include "App.h"

#include <iostream>

#include "AppUtils.h"
#include "serializer/serializer.h"
#include "serializer/serializerbuffer.h"

xsmApp::xsmApp() : mSender(xsm::Escaping::ON, *this), mReceiver(xsm::Escaping::ON, *this) {

  uint8_t buffer[xsm::MAX_PAYLOAD_SIZE];

  nowtech::SerializerBuffer serializerBuffer(buffer, xsm::MAX_PAYLOAD_SIZE);
  serializerBuffer.setAvailablaLength(xsm::MAX_PAYLOAD_SIZE);
  nowtech::Serializer serializer(serializerBuffer, 0, 0);

  serializer << "hello world! " << 42 << 42.42F << static_cast<uint8_t>(0b00101111);
  xsm::Message msg;
  msg.Size = serializer.getLength();
  serializer.reset();
  serializer.read(msg.Data.data(), msg.Size);
  mSender.send(msg);

  serializer.reset();
  serializer << "another message with string and numbers! " << 3 << 3.14F << 'c';
  msg.Size = serializer.getLength();
  serializer.reset();
  serializer.read(msg.Data.data(), msg.Size);
  mSender.send(msg);


  // xsm::Message msg1{{'a', 'b', 'c', 'd'}, 4};
  // msg1.Data[3] = xsm::FRAME_DELIMITER; // test escaping
  // mSender.send(msg1);

  // xsm::Message msg2{{'a', 'b', 'c', 'd'}, 4};
  // msg2.Data[3] = xsm::ESCAPE_BYTE; // test escaping
  // mSender.send(msg2);
}

void xsmApp::send(const uint8_t* data, const size_t size) {
  for (size_t i = 0; i < size; ++i) {
    mReceiver.receive(data[i]);
  }
}

void xsmApp::onMessageReceived(const xsm::Message& message) {
  // std::cout << "message received" << std::endl;
  AppUtils::print(message.Data.data(), message.Size);
  std::cout << std::endl;

  uint8_t buffer[xsm::MAX_PAYLOAD_SIZE];
  nowtech::SerializerBuffer serializerBuffer(buffer, xsm::MAX_PAYLOAD_SIZE);
  serializerBuffer.setAvailablaLength(xsm::MAX_PAYLOAD_SIZE);
  nowtech::Serializer serializer(serializerBuffer, 0, 0);
  serializerBuffer.setData(message.Data.data(), message.Size);

  char stringBuffer[1024];
  int i;
  float f;
  uint8_t c;
  serializer >> stringBuffer >> i >> f >> c;

  std::cout << "message received: " << stringBuffer << i << " " << f << c << std::endl;
  std::cout << std::endl;
}
