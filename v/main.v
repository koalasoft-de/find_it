import io
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

	mut out     := os.open_append(leaked_file)!
	mut threads := []thread{}
	mut lines   := unsafe { &u32(malloc(4)) }
 
	for file in os.ls('./')! {

		if !file.contains('log.') {
			continue
		}

		threads << spawn scan_log(mut &out, file, passwords, lines)
	}

  threads.wait()

  print('Scanned ${*lines} lines')

	out.close()
}

fn scan_log(mut out os.File, file string, passwords []string, lines &u32) {

	mut log := io.new_buffered_reader(
		reader: os.open(file) or { panic(err) }
	)
  
	mut local_lines := u32(0)

	for {
		line := log.read_line() or { break } 

		local_lines++
		
		for pw in passwords {

			if line.index_after(pw, 0) > -1 {
				i1 := line.index(': ') or { panic(err) } + 2
				i2 := line.index_after('failed', i1) - 1

				res := 'Leak found: ${line.substr(i1, i2)} | Password: ${pw} | Log File: ${file}'

				println(res)

				out.writeln(res) or { panic(err) }
			}
		}
	}

	(*lines) += local_lines;
}