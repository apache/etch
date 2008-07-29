ufind . ( -name .svn -prune ) -o ( -name build -prune ) -o -type f -exec svn propget svn:eol-style {} ; -print > snot
unix2dos snot
