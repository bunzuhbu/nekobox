source script/env_deploy.sh

pushd "$SRC_ROOT"/core/server
mkdir -p "$DEPLOYMENT/gen"
pushd gen
  protoc -I . --go_out="$DEPLOYMENT/gen" --protorpc_out="$DEPLOYMENT/gen" libcore.proto
popd
go get github.com/chai2010/protorpc
go get github.com/golang/protobuf/proto
go mod tidy
go mod vendor
mv -T vendor "$DEPLOYMENT/vendor"
curl https://api.github.com/repos/sagernet/sing-box/releases/latest | jq -r '.name' > "$DEPLOYMENT/Sagernet.SingBox.Version.txt"
popd

