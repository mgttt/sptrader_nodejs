['zlib','fs'].forEach((c,i,a)=>{global[c]=require(c);});
var dt=new Date(),yyyy=dt.getFullYear(),mm=dt.getMonth()+1,dd=dt.getDate(),logname=(process.argv[2]||"")+`-${yyyy}-${mm}-${dd}.gz`;
process.stdin.on('readable',()=>{if(chunk=process.stdin.read()){process.stdout.write(chunk);fs.appendFileSync(logname,zlib.gzipSync(Buffer.from(chunk)))}});
