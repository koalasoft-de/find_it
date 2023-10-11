import os

const (
	leaked_file = './leaked.txt'
)

fn main() {
  passwords := os.read_lines('./passwords.txt')!

  if os.exists(leaked_file) {
		os.rm(leaked_file)!
	}

	os.create(leaked_file)!

	mut out := os.open_append(leaked_file)!
 
	for f in os.ls('./')! {
		mut log := os.open(f)!

		if !f.contains('log.') {
			continue
		}

    for {
		  mut buffer := []u8{len: 256}

			if log.read_bytes_into_newline(mut buffer)! > 0 {
     
			  for pw in passwords {
				  line := buffer.bytestr()
 
          if line.contains(pw) {
            i1 := line.index(': ')? + 2
  	 				i2 := line.index_after('failed', i1) - 1
 
					  res := 'Leak found: ${line.substr(i1, i2)} | Password: ${pw} | Log File: ${f}'

					  println(res)

					  out.writeln(res)!
				  }
			  }
			} else {
				break
			}
		}

		log.close()
	}

	out.close()
}