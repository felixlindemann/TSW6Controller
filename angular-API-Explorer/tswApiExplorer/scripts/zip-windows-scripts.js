const fs = require('fs');
const path = require('path');
const archiver = require('archiver');

// Verzeichnisse
const windowsDir = path.resolve(__dirname, '../../../TSW6_Controller/Windows');
const publicDir = path.resolve(__dirname, '../public');
const zipName = 'TSW6_Windows_Skripte.zip';
const zipPath = path.join(publicDir, zipName);

// Sicherstellen, dass das public-Verzeichnis existiert
if (!fs.existsSync(publicDir)) {
  fs.mkdirSync(publicDir, { recursive: true });
}

// Nur .cmd und .ps1 Dateien sammeln
const files = fs.readdirSync(windowsDir).filter(f => f.endsWith('.cmd') || f.endsWith('.ps1'));

if (files.length === 0) {
  console.log('Keine .cmd oder .ps1 Dateien im Windows-Verzeichnis gefunden.');
  process.exit(0);
}

const output = fs.createWriteStream(zipPath);
const archive = archiver('zip', { zlib: { level: 9 } });

output.on('close', function() {
  console.log(`ZIP erfolgreich erstellt: ${zipPath} (${archive.pointer()} bytes)`);
});

archive.on('error', function(err) {
  throw err;
});

archive.pipe(output);

files.forEach(file => {
  archive.file(path.join(windowsDir, file), { name: file });
});

archive.finalize();
