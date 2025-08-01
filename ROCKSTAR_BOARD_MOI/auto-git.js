const chokidar = require('chokidar');
const { exec } = require('child_process');

// Theo dõi thay đổi trong thư mục
const watcher = chokidar.watch('.', {
  ignored: /(^|[\/\\])(\..|node_modules)/, // bỏ qua .git, .vscode, node_modules
  persistent: true,
});

let timeout;

watcher.on('all', (event, path) => {
  clearTimeout(timeout);
  timeout = setTimeout(() => {
    console.log(`📂 Có thay đổi ở: ${path}`);
    exec('git add . && git commit -m "Auto commit" && git push', (err, stdout, stderr) => {
      if (err) {
        console.error(`❌ Lỗi khi push: ${stderr}`);
      } else {
        console.log('✅ Đã commit và push thành công');
      }
    });
  }, 3000); // chờ 3 giây sau thay đổi
});
