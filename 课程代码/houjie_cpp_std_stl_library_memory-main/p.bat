java j.p_bat
rem mv D:\gitHub\java_ubuntu_windows\g\g.exe d:\path\g.exe
rem rm D:\gitHub\java_ubuntu_windows\g\g.exe
rem sudo chmod -R 777 ./
set http_proxy=http://127.0.0.1:10809
set https_proxy=http://127.0.0.1:10809
rem cd D:\gitHub\ubuntu_java\
rem chmod a+x *.sh
rem :loop
call bat\java_ubuntu_windows_dependency.bat
git pull
git add .
git commit -m "a"
git push
rem java j.copy_zip_001_push
rem goto loop
