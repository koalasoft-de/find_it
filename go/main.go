package main

import (
	"bufio"
	"fmt"
	"os"
	"strings"
)

func main() {
	files, _ := os.ReadDir("./")

	os.Remove("./leaked.txt")

	pwFile, _ := os.ReadFile("./passwords.txt")
	passwords := strings.Split(string(pwFile), "\n")
	channels := []chan int{}
	leaked, _ := os.OpenFile("./leaked.txt", os.O_APPEND|os.O_WRONLY|os.O_CREATE, 0666)

	defer leaked.Close()

	for _, file := range files {

		if !strings.HasPrefix(file.Name(), "log.") {
			continue
		}

		c := make(chan int)

		channels = append(channels, c)

		go scanLog(c, file.Name(), leaked, passwords)
	}

	lines := 0

	for _, c := range channels {
		lines += <-c
	}

	fmt.Printf("Scanned %v lines\n", lines)
}

func scanLog(c chan int, file string, out *os.File, passwords []string) {
	log, _ := os.Open(file)
	lines := 0

	defer log.Close()

	scanner := bufio.NewScanner(log)

	for scanner.Scan() {
		lines++

		for _, pw := range passwords {
			line := scanner.Text()

			if line == "" || pw == "" {
				continue
			}

			if strings.Contains(line, pw) {
				i1 := strings.Index(line, ": ") + 2
				i2 := strings.Index(line, "failed") - 1

				res := fmt.Sprintf("Leak found: %v | Password: %v | Log File: %v\n", line[i1:i2], pw, file)

				fmt.Print(res)
				out.WriteString(res)
			}
		}
	}

	c <- lines
}
