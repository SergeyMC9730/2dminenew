rm proto/gen -rf
mkdir proto/gen
protoc --proto_path=proto --cpp_out=proto/gen --experimental_allow_proto3_optional proto/TestPacket.proto proto/MetaPacket.proto proto/ClientRegisterPacket.proto proto/BallInfo.proto proto/BallDisconnect.proto
