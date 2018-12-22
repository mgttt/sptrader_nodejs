{
	'targets': [{
		"target_name":"SpTrader" ,'sources': [ 'Addon.cc','AddonLogic.hpp' ]
			,"include_dirs": [ "include" ]
			,'libraries': ['-L.','-lapiwrapper']

			,'xcode_settings': {
				'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
					'OTHER_CFLAGS': [ '-fexceptions', '-frtti', ],
			}

		,"cflags": [ "-g3", "-ggdb", "-O0" ]
		,"cflags_cc": ["-g3","-c","-O3","-std=c++0x","-ggdb","-fexceptions","-frtti","-D__LINUX__=1","-DBOOST_MODE=1","-DBOOST_UTILS=1","-D_DEBUG=1"]
	}]
}
