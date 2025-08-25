source script/env_deploy.sh

pushd "$SRC_ROOT"/core/server
pushd gen
  protoc -I . --go_out="$DEPLOYMENT/gen" --protorpc_out="$DEPLOYMENT/gen" libcore.proto
popd
go mod vendor
mkdir -p "$DEPLOYMENT/gen"
mv -T vendor "$DEPLOYMENT/vendor"
curl https://api.github.com/repos/sagernet/sing-box/releases/latest | jq -r '.name' > "$DEPLOYMENT/Sagernet.SingBox.Version.txt"
popd

