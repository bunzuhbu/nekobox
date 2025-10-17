package main

import (
	"Core/gen"
	"os"
)

func (s *server) SetSystemDNS(in *gen.SetSystemDNSRequest, out *gen.EmptyResp) error {
	return nil
}

func runAdmin(_port * int, _debug * bool) int {
	return 0
}

func (s *server) IsPrivileged(in *gen.EmptyReq, out *gen.IsPrivilegedResponse) error {
	out.HasPrivilege = To(os.Geteuid() == 0)
	return nil
}

func WaitForProcessExit (pid int) error{
	return nil
}
