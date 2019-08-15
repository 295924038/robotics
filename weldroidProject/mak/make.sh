#!/bin/bash

#多个值要用双引号包含！！！！！！！！！！！！！！

#指定生成动态库dynamic  静态库static 可执行目标exec 对应的debug和release方式及JAVA字节码java
targetType=

#指定目标列表  如：test  动态库会自动扩展为libtest.so 静态库同理
targets=

#指定目标依赖
#命名规则  目标_ojbs  目标_libs  如:test_objs  test_libs test_includepath
device_objs=
device_libs=
xxx_includepath=


#下面的代码不要修改!!!!!!!!!!!!!!!!!!!!!!!!

#默认为debug方式
if [ 0 -eq $# ]
then
   cmds=debug
else
   cmds=$@
fi

make_func()
{
   if [ ! -d "depend" ]
   then
      mkdir "depend"
   fi

   if [ ! -d "obj" ]
   then
      mkdir "obj"
   fi

   for target in ${targets}
   do
      for cmd in $@
      do
	 #支持默认目标文件
	 export STARGET=${target} 
	 export SOBJS="`eval echo '$'{${target}_objs'}'`"
         if [ -z "${SOBJS}" ]
         then
           export   SOBJS="${target}.o"
         fi

	 export SLIBS="`eval echo '$'{${target}_libs'}'`" 
         export SINCLUDEPATH="`eval echo '$'{${target}_includepath'}'`"

         if [ "clean" = ${cmd} ]
         then
            make -f ${PROROOT}/mak/mkcore.mak ${cmd} 
         elif [ "all" = ${cmd} ]
         then
            cmd=debug${targetType}
            make -f ${PROROOT}/mak/mkcore.mak ${cmd}
         else
            cmd=${cmd}${targetType}
            make -f ${PROROOT}/mak/mkcore.mak ${cmd}
         fi
      done
   done
}

for cmd in ${cmds}
do
   make_func ${cmd}
done
