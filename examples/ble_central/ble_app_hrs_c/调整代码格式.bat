echo   off
REM =================================================================
REM   使用Astyle整理代码格式
REM   版本：v1.0   日期：2012-2-28
REM   作者：LiuNing
REM   说明：将此批处理文件复制到需要整理代码目录的根路径，执行既可
REM         执行完成后，被整理格式的文件将产生*.orig备份文件
REM         可以根据提示选择是否删除备份文件
REM         所有*.c *.h代码格式转换成allman格式
REM =================================================================
echo   ----------------------------------------------------------
echo   ---  【Astyle代码整理批处理文件(v1.0)】 By : LiuNing    --
echo   --- 确定要将当前目录及子目录中*.c和*.h转换为allman格式  --
echo   ----------------------------------------------------------
set /p DoAstyle="确定转换？（Y/N）"
if     %DoAstyle%==y  (goto StartAstyle) else    (goto end)

:StartAstyle
echo   Astyle 正在整理代码格式...
REM    遍历当前文件夹下的所有目录子目录中的文件，找出*.c和*.h文件。
for /r  %%I in (*.c *.h) do (
	REM    echo  %%I
	REM    执行astyle.exe调整代码格式为allman格式
	astyle.exe --style=allman "%%I"
	REM    若需要将代码格式转换为java格式，使用下面语句
	REM    astyle.exe --style=java "%%I"
)

REM    astyle转换代码后会自动备份代码文件到*.orig文件
REM    现在确定是否需要删除astyle生成的*.orig文件
echo   ----------------------------------------------------------
echo   --  Astyle代码整理以完成，是否需要保留备份文件"*.orig"? --
echo   ----------------------------------------------------------
set /p Del_orig="是否保留？（Y/N）"
if     %Del_orig%==y  (goto end) else    (goto DelORIG)

:DelORIG
REM    遍历当前文件夹下的所有目录子目录中的文件，找出*.orig文件。
for /r  %%I in (*.orig) do (
	REM    echo  %%I
	REM    删除找到的*.orig文件
	del "%%I"
)
REM    这里已完成备份文件*.orig的清理
echo   ----------------------------------------------------------
echo   --        备份文件*.orig文件清理结束！                  --
echo   ----------------------------------------------------------
:end
REM    到这里代码整理结束
echo   ---------- Astyle代码格式整理完毕！ ----------------------
echo   按任意键退出 & pause
