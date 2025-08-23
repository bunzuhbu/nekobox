#!/bin/bash -x

cat << EOF > oscrc
[general]
apiurl=https://api.opensuse.org
[https://api.opensuse.org]
user="${USERNAME}"
pass="${PASSWORD}"
credentials_mgr_class=osc.credentials.PlaintextConfigFileCredentialsManager
EOF

install -Dm644 oscrc "$HOME"/.config/osc/oscrc

osc token --trigger "${TOKEN}"
