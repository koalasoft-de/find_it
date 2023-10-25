use std::fs;
use std::io::{self, BufRead, Write};
use std::thread;
use std::sync::{Arc, Mutex};

fn main() -> io::Result<()> {
	let dir = fs::read_dir("./")?;

	fs::remove_file("./leaked.txt").ok();

	let pw_file = fs::read_to_string("./passwords.txt")?;
	let passwords: Vec<String> = pw_file.split('\n').map(|s| s.to_string()).collect();
	let handles = Arc::new(Mutex::new(Vec::new()));
	let leaked = Arc::new(Mutex::new(fs::OpenOptions::new()
		.create(true)
		.write(true)
		.append(true)
		.open("./leaked.txt")?));

	let mut threads = vec![];

	for entry in dir {
		if let Ok(file) = entry {
			if !file.file_name().to_string_lossy().starts_with("log.") {
				continue;
			}

			let handles_clone = Arc::clone(&handles);
			let leaked_clone = Arc::clone(&leaked);
			let passwords_clone = passwords.clone(); // Clone passwords

			let thread = thread::spawn(move || {
				let lines = scan_log(file.file_name(), &leaked_clone, &passwords_clone); // Use the clone
				let mut handles = handles_clone.lock().unwrap();
				handles.push(lines);
			});

			threads.push(thread);
		}
	}

	for thread in threads {
		thread.join().unwrap();
	}

	let total_lines: usize = handles.lock().unwrap().iter().sum();

	println!("Scanned {} lines", total_lines);

	Ok(())
}

fn scan_log(file: std::ffi::OsString, leaked: &Mutex<fs::File>, passwords: &Vec<String>) -> usize {
	let log = fs::File::open(&file).expect("Failed to open log file");
	let reader = io::BufReader::new(log);
	let mut lines = 0;

	for line in reader.lines() {
		lines += 1;
		let line = line.expect("Failed to read line");

		for pw in passwords.iter() {
			let line = line.trim();
			if line.is_empty() || pw.is_empty() {
				continue;
			}

			if line.contains(pw) {
				if let Some(i1) = line.find(": ") {
					if let Some(i2) = line.find("failed") {
						let res = format!(
							"Leak found: {} | Password: {} | Log File: {}\n",
							&line[i1 + 2..i2 - 1],
							pw,
							file.to_string_lossy()
						);

						print!("{}", res);
						let mut leaked = leaked.lock().unwrap();
						leaked.write_all(res.as_bytes()).expect("Failed to write to the output file");
					}
				}
			}
		}
	}

	lines
}
