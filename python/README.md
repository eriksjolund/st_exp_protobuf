First install python-protobuf

(On Ubuntu linux run:
apt-get install python-protobuf
)

protoc -I=../protobuf_schema --python_out=/tmp/out ../protobuf_schema/fileformat_common.proto
protoc -I=../protobuf_schema --python_out=/tmp/out ../protobuf_schema/tiled_image.proto
protoc -I=../protobuf_schema --python_out=/tmp/out ../protobuf_schema/tiled_image.proto
protoc -I=../protobuf_schema --python_out=/tmp/out ../protobuf_schema/st_exp.proto

