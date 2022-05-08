@cd %~dp0

@call gradlew clean deps

@echo start ...
set JAVA_OPTS=-Dgroovy.attach.runtime.groovydoc=true %*
cd src/
../bin/groovy -pa main.groovy