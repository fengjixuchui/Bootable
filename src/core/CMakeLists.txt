cmake_minimum_required(VERSION 2.6)

add_library("bootable-core"
	"dap.c"
	"dir.c"
	"error.c"
	"file.c"
	"fs.c"
	"gpt.c"
	"mbr.c"
	"misc.c"
	"partition.c"
	"path.c"
	"stream.c"
	"string.c"
	"uuid.c")
