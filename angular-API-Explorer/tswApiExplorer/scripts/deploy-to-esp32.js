#!/usr/bin/env node

const fs = require('fs');
const path = require('path');

// Pfade
const sourceDir = path.join(__dirname, '../dist/tsw-api-explorer/browser');
const targetDir = path.join(__dirname, '../../../TSW6_Controller/data/www');

console.log('🚀 Deploying Angular App to ESP32...');
console.log(`   Source: ${sourceDir}`);
console.log(`   Target: ${targetDir}`);

// Prüfe ob Build existiert
if (!fs.existsSync(sourceDir)) {
  console.error('❌ Build not found! Run "npm run build" first.');
  process.exit(1);
}

// Erstelle Zielverzeichnis
if (!fs.existsSync(targetDir)) {
  fs.mkdirSync(targetDir, { recursive: true });
  console.log('✅ Created target directory');
}

// Kopiere rekursiv
function copyRecursive(src, dest) {
  const stats = fs.statSync(src);
  
  if (stats.isDirectory()) {
    if (!fs.existsSync(dest)) {
      fs.mkdirSync(dest, { recursive: true });
    }
    const files = fs.readdirSync(src);
    files.forEach(file => {
      copyRecursive(path.join(src, file), path.join(dest, file));
    });
  } else {
    fs.copyFileSync(src, dest);
    const size = (stats.size / 1024).toFixed(2);
    console.log(`   📄 ${path.basename(dest)} (${size} KB)`);
  }
}

// Lösche altes Verzeichnis
if (fs.existsSync(targetDir)) {
  fs.rmSync(targetDir, { recursive: true, force: true });
  fs.mkdirSync(targetDir, { recursive: true });
}

// Kopiere Dateien
copyRecursive(sourceDir, targetDir);

// Statistik
const files = fs.readdirSync(targetDir);
const totalSize = files.reduce((sum, file) => {
  const stats = fs.statSync(path.join(targetDir, file));
  return sum + (stats.isFile() ? stats.size : 0);
}, 0);

console.log('');
console.log('✅ Deployment complete!');
console.log(`   Files: ${files.length}`);
console.log(`   Total size: ${(totalSize / 1024).toFixed(2)} KB`);
console.log('');
console.log('📝 Next steps:');
console.log('   1. cd ../../TSW6_Controller');
console.log('   2. pio run -t uploadfs  (upload filesystem)');
console.log('   3. pio run -t upload    (upload firmware)');
