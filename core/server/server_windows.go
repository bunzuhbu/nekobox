package main

import (
	"Core/gen"
	"Core/internal/boxdns"
	"os/exec"
    "crypto/rand"
    "fmt"
    "math/big"
	"github.com/natefinch/npipe"
	"io"
	"os"
	"sync"
	"log"
)

const letters = "abcdefghijklmnopqrstuvwxyz"

func RandString(n int) (string, error) {
    result := make([]byte, n)
    for i := 0; i < n; i++ {
        num, err := rand.Int(rand.Reader, big.NewInt(int64(len(letters))))
        if err != nil {
            return "", err
        }
        result[i] = letters[num.Int64()]
    }
    return string(result), nil
}

func (s *server) SetSystemDNS(in *gen.SetSystemDNSRequest, out *gen.EmptyResp) error {
	err := boxdns.DnsManagerInstance.SetSystemDNS(nil, *in.Clear)
	if err != nil {
		return err
	}

	return nil
}


func handlePipe(pipeName string, output io.Writer, wg *sync.WaitGroup) {
	defer wg.Done()

	ln, err := npipe.Listen(pipeName)
	if err != nil {
		log.Fatalf("Error listening on pipe %s: %v\n", pipeName, err)
	}
	defer ln.Close()

	log.Printf("Waiting for client on pipe: %s\n", pipeName)
	conn, err := ln.Accept()
	if err != nil {
		log.Printf("Accept error on %s: %v\n", pipeName, err)
		return
	}
	defer conn.Close()

	fmt.Printf("Client connected to %s\n", pipeName)

	_, err = io.Copy(output, conn)
	if err != nil {
		log.Printf("Error copying from %s: %v\n", pipeName, err)
	}
}

func runAdmin(_port * int, _debug * bool) int {
	randstr, _ := RandString(6);
	
	stdout_pipe := `\\.\pipe\nekobox_core_output_pipe_` + randstr
	stderr_pipe := `\\.\pipe\nekobox_core_error_pipe_` + randstr
	flag := ""
	if (*_debug){
		flag = " \"-debug\""
	}
	formattedString := fmt.Sprintf("Start-Process \"%s\" -ArgumentList '-port %d -redirect-output \"%s\" -redirect-error \"%s\"%s' -WindowStyle Hidden -Verb RunAs -Wait", 
		os.Args[0], *_port, stdout_pipe, stderr_pipe, flag)
	
	var wg sync.WaitGroup

	// Pipe for stdout
	wg.Add(1)
	go handlePipe(stdout_pipe, os.Stdout, &wg)

	// Pipe for stderr
	wg.Add(1)
	go handlePipe(stderr_pipe, os.Stderr, &wg)

	
	cmd := exec.Command("powershell", "-Command", formattedString)
	err := cmd.Run()
	
	var code int
	
	if err != nil {
        // Process exited with error
        if exitErr, ok := err.(*exec.ExitError); ok {
            code = exitErr.ExitCode()
        } else {
			code = -1
		}
    } else {
        // Process exited successfully
        code = 0
    }
	
	return code
}