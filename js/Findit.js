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

        if (line.includes(pw)) {
          await leakedFile.write(line + '\n');
        }
      }
    }

    await fileHandle.close();
}

await leakedFile.close();

process.exit(0);
