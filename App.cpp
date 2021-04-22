#include "App.h"

#include <iostream>

#include "AppUtils.h"
#include "serializer/serializer.h"
#include "serializer/serializerbuffer.h"

Protocol::Protocol(xsm::Sender& sender) : mSender(sender) {}

void Protocol::onMessageReceived(const xsm::Message message) {
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


Connection::Connection() {}
void Connection::send(const uint8_t* data, const size_t size) {
  for (size_t i = 0; i < size; ++i) {
    mReceiver->receive(data[i]);
  }
}

void Connection::setReceiver(xsm::Receiver* receiver) {
  mReceiver = receiver;
}
