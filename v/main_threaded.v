import os

const (
	leaked_file = './leaked.txt'
)

fn read(f string, passwords []string) {
	mut log := os.open(f) or { panic('Shit') }

	for {
		mut buffer := []u8{len: 1024}

		if log.read_bytes_into_newline(mut buffer) or { panic('Shit') } > 0 {
		
		for pw in passwords {
			line := buffer.bytestr()

			if line.contains(pw) {
				i1 := line.index(': ') or { panic('Shit') } + 2
				i2 := line.index('failed') or { panic('Shit') } - 1

				mut out := os.open_append(leaked_file) or { panic('Shit') }
				ok := 'Leak found: ${line.substr(i1, i2)} | Password: ${pw} | Log File: ${f}'

				println(ok)

				out.writeln(ok) or { panic('Shit') }

				out.close()
			}
		}

		} else {
			break
		}
	}

	log.close()
}

fn main() {
  logs := os.ls('./')!.filter(it.contains('log.'))
  passwords := os.read_lines('./passwords.txt')!

  if os.exists(leaked_file) {
		os.rm(leaked_file)!
	}

	os.create(leaked_file)!

	mut threads := []thread{}

	for f in logs {
		threads << go read(f, passwords);
	}

	for t in threads {
		t.wait()
	}
}