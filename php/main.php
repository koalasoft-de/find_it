<?php

define('LEAKED', './leaked.txt');

$passwords = explode("\n", file_get_contents('./passwords.txt'));

function read_lines($file_handle) { 
	while (!feof($file_handle)) {
		yield fgets($file_handle);
	}
}

if (file_exists(LEAKED)) {
	unlink(LEAKED);
}

file_put_contents(LEAKED, '');

if ($handle = opendir('.')) {
	while (false !== ($f = readdir($handle))) {
		if (str_contains($f, 'log.')) {
			foreach (read_lines(fopen($f, 'r')) as $line) {
				
				foreach ($passwords as $pw) {
					if (empty($pw)) continue;

					if (str_contains($line, $pw)) {
						$i1   = strpos($line, ': ') + 1;
						$i2   = strpos($line, 'failed') - 1;
						$name = substr($line, $i1, $i2 - $i1);
						$res  = "Leak found: $name | Password: $pw | Log File: $f";

						print $res . "\n";

						file_put_contents(LEAKED, "$res\n", FILE_APPEND);
					}
				}

			}
		}
	}

	closedir($handle);
}