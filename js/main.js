import { open, readdir, readFile, writeFile } from 'fs/promises';

const passwordFile = await readFile('./passwords.txt', { encoding: 'utf-8' });
const passwords = passwordFile.split('\n');    

await writeFile('./leaked.txt', '').catch(_ => null);

const leakedFile = await open('./leaked.txt', 'a');

for (const file of await readdir('./')) {

    if (!file.startsWith('log.')) {
      continue;
    }

    const fileHandle = await open(file);

    for await (const line of fileHandle.readLines()) {

      for (const pw of passwords) {

        if (!pw) {
          continue;
        }

        if (line.includes(pw)) {
          const i1  = line.indexOf(': ') + 2,
				        i2  = line.indexOf('failed', i1) - 1,
                res = `Leak found: ${line.substring(i1, i2)} | Password: ${pw} | Log File: ${file}`;

          console.log(res);

          await leakedFile.write(res + '\n');
        }
      }
    }

    await fileHandle.close();
}

await leakedFile.close();

process.exit(0);
