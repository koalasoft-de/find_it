import os

const (
	leaked_file = './leaked.txt'
)

fn main() {
  logs := os.ls('./')!.filter(it.contains('log.'))
  passwords := os.read_lines('./passwords.txt')!

  if os.exists(leaked_file) {
		os.rm(leaked_file)!
	}

	os.create(leaked_file)!

	for f in logs {
		mut log   := os.open(f)!
		mut lines := 0

    for {
		  mut buffer := []u8{len: 1024}

			if log.read_bytes_into_newline(mut buffer)! > 0 {
			 lines++;
       
			 for pw in passwords {
				 line := buffer.bytestr()

         if line.contains(pw) {
           i1 := line.index(': ')? + 2
					 i2 := line.index('failed')?

					 mut out := os.open_append(leaked_file)!
					 ok := 'Leak found: ${line.substr(i1, i2)} | Password: ${pw} | Log File: ${f}'

					 println(ok)

					 out.writeln(ok)!

					 out.close()
				 }
			 }

			} else {
				break
			}
		}

		log.close()
	}
}