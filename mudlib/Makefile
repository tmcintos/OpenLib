clean:
	find mudlib -name '*~' -exec rm '{}' ';'

archive.mudlib:
	tar --exclude "mudlib/distrib/*" -zvcf archive/`date "+mudlib.%y.%m.%d.tgz"` mudlib

distribution:
	tar --exclude "mudlib/u/*/*" \
	--exclude "*.o" \
	--exclude "*.html" \
	--exclude "mudlib/doc/driver*" \
	--exclude "mudlib/doc/projects*" \
	--exclude "mudlib/log/*" \
	--exclude "mudlib/distrib*" \
	--exclude "mudlib/tmp/*" \
	--exclude "mudlib/adm/tmp/*" \
	--exclude "mudlib/adm/news/*" \
	--exclude "mudlib/ftp/pub/*" \
	--exclude "mudlib/ftp/incoming/*" \
	-zvcf `date "+OpenLib.dist.%y.%m.%d.tgz` mudlib
