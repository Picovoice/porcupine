const http = require('http');
const fs = require('fs');
const path = require('path');
const mime = require('mime-types');

const PORT = process.env.PORT || 5000;
const HOST = '127.0.0.1';  // Listen on localhost
const publicDir = path.join(__dirname);

const server = http.createServer((req, res) => {
  const urlPath = req.url.split('?')[0];
  const url = (urlPath === '/') ? '/index.html' : urlPath;
  const filePath = path.join(publicDir, url);
  const contentType = mime.lookup(filePath) || 'application/octet-stream';

  fs.readFile(filePath, (err, content) => {
    if (err) {
      if (err.code === 'ENOENT') {
        // File not found
        res.writeHead(404, { 'Content-Type': 'text/plain' });
        res.end('404 Not Found');
      } else {
        // Server error
        res.writeHead(500, { 'Content-Type': 'text/plain' });
        res.end(`500 Internal Server Error: ${err.code}`);
      }
    } else {
      // Success
      res.writeHead(200, {
        'Content-Type': contentType,
        'Content-Length': content.length,
        'Cross-Origin-Opener-Policy': 'same-origin',
        'Cross-Origin-Embedder-Policy': 'require-corp'
      });
      res.end(content);
    }
  });
});

server.listen(PORT, HOST, () => {
  console.log(`Server is running on http://${HOST}:${PORT}`);
});