pushd %~dp0

protoc.exe -I=./ --cpp_out=./ ./Enum.proto
protoc.exe -I=./ --cpp_out=./ ./Struct.proto
protoc.exe -I=./ --cpp_out=./ ./Protocol.proto

GenPackets.exe --path=./protocol.proto --output=ServerPacketHandler --recv=C_ --send=S_
GenPackets.exe --path=./protocol.proto --output=ClientPacketHandler --recv=S_ --send=C_

IF ERRORLEVEL 1 PAUSE

XCOPY /Y Enum.pb.h "../../../GameServer"
XCOPY /Y Enum.pb.cc "../../../GameServer"
XCOPY /Y Struct.pb.h "../../../GameServer"
XCOPY /Y Struct.pb.cc "../../../GameServer"
XCOPY /Y Protocol.pb.h "../../../GameServer"
XCOPY /Y Protocol.pb.cc "../../../GameServer"
XCOPY /Y ServerPacketHandler.h "../../../GameServer"

XCOPY /Y Enum.pb.h "../../../DummyClient"
XCOPY /Y Enum.pb.cc "../../../DummyClient"
XCOPY /Y Struct.pb.h "../../../DummyClient"
XCOPY /Y Struct.pb.cc "../../../DummyClient"
XCOPY /Y Protocol.pb.h "../../../DummyClient"
XCOPY /Y Protocol.pb.cc "../../../DummyClient"
XCOPY /Y ClientPacketHandler.h "../../../DummyClient"

XCOPY /Y Enum.pb.h "../../../../IocpTest/Source/IocpTest/Network"
XCOPY /Y Enum.pb.cc "../../../../IocpTest/Source/IocpTest/Network"
XCOPY /Y Struct.pb.h "../../../../IocpTest/Source/IocpTest/Network"
XCOPY /Y Struct.pb.cc "../../../../IocpTest/Source/IocpTest/Network"
XCOPY /Y Protocol.pb.h "../../../../IocpTest/Source/IocpTest/Network"
XCOPY /Y Protocol.pb.cc "../../../../IocpTest/Source/IocpTest/Network"
XCOPY /Y ClientPacketHandler.h "../../../../IocpTest/Source/IocpTest/Network"


DEL /Q /F *pb.h
DEL /Q /F *pb.cc
DEL /Q /F *.h

PAUSE