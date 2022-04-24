# [](https://github.com/archguard/archguard/compare/v1.6.2...v) (2022-04-23)



## [1.6.2](https://github.com/archguard/archguard/compare/v1.6.0...v1.6.2) (2022-04-23)


### Bug Fixes

* add clone with branch and fixed [#36](https://github.com/archguard/archguard/issues/36) ([6fdbfb5](https://github.com/archguard/archguard/commit/6fdbfb5de5b53ad54f49894ce3ebb67123c8cdac))
* fix callers typo && fixed [#54](https://github.com/archguard/archguard/issues/54) ([13a8e43](https://github.com/archguard/archguard/commit/13a8e436c95839ac8b47245f4c68fa723b9caeab))
* fix imports ([2ad7b04](https://github.com/archguard/archguard/commit/2ad7b04a47522a1ee365310baefb75fb94d631f5))
* fix typos ([69ce509](https://github.com/archguard/archguard/commit/69ce509df2baeb1439a3d5052b3735bdacfe09ab))
* **sca:** add lost dep source ([40c485e](https://github.com/archguard/archguard/commit/40c485e0034ece7760599d5f06a641d61d18c765))
* **sca:** add lost dep version ([bcd87f7](https://github.com/archguard/archguard/commit/bcd87f759e81f627ff67cf6a1f1566bb822b75d6))
* Service naming conforms to k8s DNS specification ([a5b1d34](https://github.com/archguard/archguard/commit/a5b1d34a5322c764e69bd9ceb4102f085e3e9b95))


### Features

* **auth:** add auth type model for [#40](https://github.com/archguard/archguard/issues/40) ([d292503](https://github.com/archguard/archguard/commit/d292503dad7787e4c40533c4243e2a99a1305edb))
* **sca:** add lost version ([ce94715](https://github.com/archguard/archguard/commit/ce947155f1f77eebd2993778b080ddf3743b12a6))
* **sca:** fix dto jdbi issue ([dc2a38c](https://github.com/archguard/archguard/commit/dc2a38c287a503282e7e87f8d445a6f24eeaa9c8))
* **sca:** init project controller and api ([ba01b16](https://github.com/archguard/archguard/commit/ba01b1600c681a3e60f316a5308bc79485c105f3))
* **sca:** init project table ([0153b67](https://github.com/archguard/archguard/commit/0153b678c37e386bfb11a363d480613aeceedbe6))
* **sca:** init tool and scanner for scan ([cbd1fdb](https://github.com/archguard/archguard/commit/cbd1fdb35de179fc3f7cfe0245cd98f71d5dbb68))



# [1.6.0](https://github.com/archguard/archguard/compare/v1.5.4...v1.6.0) (2022-04-13)


### Bug Fixes

* **diff:** fix change dbUrl issue ([bc2584d](https://github.com/archguard/archguard/commit/bc2584dc0e2cc0b822629d7a8e994179cd42f3e8))
* **diff:** fix scanner branch issue ([f32f90b](https://github.com/archguard/archguard/commit/f32f90b48b4ab786564dc515737f1e05bd48e7b2))
* fix typos for version issues ([138cb6f](https://github.com/archguard/archguard/commit/138cb6fae2af60714d45d3c2ccc1df44d5b4ef9f))
* **git:** clean dir when is not empty ([c01eec0](https://github.com/archguard/archguard/commit/c01eec0c09224c43da2579be60831266546a6ccd))
* **git:** mkdir after clear ([58d6681](https://github.com/archguard/archguard/commit/58d6681583c349b7d24058a0e4deed1031db4e3e))
* **scanner:** fix test bad smell not scan for test issue ([62a2eb6](https://github.com/archguard/archguard/commit/62a2eb6506c0f0597ab7731b61ebbf2f6b849edd))
* **scm:** fix committer_email length issue for [#33](https://github.com/archguard/archguard/issues/33) ([2bbecac](https://github.com/archguard/archguard/commit/2bbecac14106ad601aea9f010391c601a7d84d13))


### Features

* **change:** add history api for query ([dbf1404](https://github.com/archguard/archguard/commit/dbf140434008b83304e517206be0cdde267ca821))
* **change:** add send back results after analysis ([150b175](https://github.com/archguard/archguard/commit/150b175739ca17beb6b2299612b3cf40e1d9025b))
* **diff:** add arguments for tools ([8ec1456](https://github.com/archguard/archguard/commit/8ec1456bd449051d8aefd6de15519f30903b3778))
* **diff:** add since and util support [#19](https://github.com/archguard/archguard/issues/19) ([fa5f125](https://github.com/archguard/archguard/commit/fa5f1255b193155e30bf05f24e35a07017ebc9d9))
* **diff:** disabled for can scan ([f6e675f](https://github.com/archguard/archguard/commit/f6e675f46d773ffe794b49eb9f8dfabe5e9ec505))
* **diff:** make diff api to call scanner tool ([fa7af7f](https://github.com/archguard/archguard/commit/fa7af7fa1dfde2a5e500a3e4d814fb313f346fc0))
* enable arguments for scanner tools ([1e6493b](https://github.com/archguard/archguard/commit/1e6493b2fac4b96b74a4e80a62e052f550b3489b))
* **git:** enable kill tmp for save history ([2233449](https://github.com/archguard/archguard/commit/22334498351366eb6ffb7c224707eb57ddd73860))



## [1.5.4](https://github.com/archguard/archguard/compare/v1.5.3...v1.5.4) (2022-04-10)


### Bug Fixes

* fix not clean sql issues for [#16](https://github.com/archguard/archguard/issues/16) ([6fcc8af](https://github.com/archguard/archguard/commit/6fcc8af309a65e40ee0b1bd83ed0264dc13cdd95))
* fix when over generalization empty issues ([30cc253](https://github.com/archguard/archguard/commit/30cc25302de6357448ef2c1da887169d04f76a9a))


### Features

* add handle for error catch ([b534293](https://github.com/archguard/archguard/commit/b5342933cb6838729f13a5186246eca6606c6f97))
* **git:** fix command issue for [#16](https://github.com/archguard/archguard/issues/16) ([ea6aab7](https://github.com/archguard/archguard/commit/ea6aab720a3596e5bd96eaba17034dc79464e674))



## [1.5.3](https://github.com/archguard/archguard/compare/v1.5.2...v1.5.3) (2022-04-09)


### Features

* **log:** add basic log api to load logs for scanner and [#13](https://github.com/archguard/archguard/issues/13) ([1b341cc](https://github.com/archguard/archguard/commit/1b341ccbda056a89a7607e7f89564c6fc1dd4246))
* **log:** add log for git clone and closed [#13](https://github.com/archguard/archguard/issues/13) ([73b2852](https://github.com/archguard/archguard/commit/73b2852220c28e51f98005460791b58154b667ba))
* **log:** add log stream to all scanns tool for [#13](https://github.com/archguard/archguard/issues/13) ([b24708e](https://github.com/archguard/archguard/commit/b24708eaad4fea10dfe13aa8e2b8d2cc44b58411))



## [1.5.2](https://github.com/archguard/archguard/compare/v1.5.0...v1.5.2) (2022-04-08)


### Bug Fixes

* add curl into docker image to fix health-check ([f9086c0](https://github.com/archguard/archguard/commit/f9086c0501f5acf535eaab6c4a2fee21df1548ff))
* fix annotaion key size issue ([3715089](https://github.com/archguard/archguard/commit/37150891ebe31676d6b4cee5e3b457c03b354b40))
* fix lint ([1b6b1f6](https://github.com/archguard/archguard/commit/1b6b1f69f5a90c8b2aa4bd69a7bab816e987a3e8))
* fix repo id error issues ([1c61eb1](https://github.com/archguard/archguard/commit/1c61eb13dc1cd06a83d40577a49ce0a58b2bba81))
* fix rescan not clear related data issue ([4f02415](https://github.com/archguard/archguard/commit/4f024158a7517d5c8f23c4f343b4ba71d0242c59))
* **git:** change fetch to pull rebase ([bacdd98](https://github.com/archguard/archguard/commit/bacdd98b7facbc9b949049ef9477d4a3333f5c17))


### Features

* **diff:** init db ([ed1de99](https://github.com/archguard/archguard/commit/ed1de995ddbac6adb626103d3537351a7992d692))
* **git:** add fetch and checkout for command ([554c1d7](https://github.com/archguard/archguard/commit/554c1d7f3be4e4c521db175ec6582a0f10bea22d))
* **git:** add fetch support ([1a5106d](https://github.com/archguard/archguard/commit/1a5106d89dbcd1d5efa6c04a932c8fec888308d7))
* **git:** add workdir to system table ([4c9a1a3](https://github.com/archguard/archguard/commit/4c9a1a3fe5fd5f75d6914907c2b160152e48b5d6))
* **git:** create workdir when scan start ([11eda39](https://github.com/archguard/archguard/commit/11eda399f312bc60833707dedb632f759dfb9b30))
* **git:** make dir can clone ([cc75dd9](https://github.com/archguard/archguard/commit/cc75dd9e4bdd4cab1651ba17ad89d1530efb55c0))
* init git command for home ([68be4fa](https://github.com/archguard/archguard/commit/68be4fa97f67139ce1dddcbf6919aff8c2e93e9c))
* **log:** init basic memory consumer ([5907722](https://github.com/archguard/archguard/commit/5907722b8f073deeed73c506e3371b17463f878e))



# [1.5.0](https://github.com/archguard/archguard/compare/v1.4.6...v1.5.0) (2022-04-07)


### Bug Fixes

* default branch issue ([10a424c](https://github.com/archguard/archguard/commit/10a424c80a5cf5a8aa8979bafb057741ec751b66))
* fix import issues ([5d5eb90](https://github.com/archguard/archguard/commit/5d5eb90b3a7e451dd4a42e371f338db33ddd628a))
* fix mcr package error issues ([381f17a](https://github.com/archguard/archguard/commit/381f17ac898feaecce0c35583755bdd87bebfb54))
* fix package path not change issue ([9aec403](https://github.com/archguard/archguard/commit/9aec40341ca8d9db4de8fe6787735a962a5572e2))
* use named volumes instead of bind mounts ([4d2d476](https://github.com/archguard/archguard/commit/4d2d476a3f0716d3b3c9a847b4bb98b77c91ae42))


### Features

* **change:** init ranage api ([6222c89](https://github.com/archguard/archguard/commit/6222c89b622da4440725c6841b748299e64f785d))
* install.sh ([37bdffb](https://github.com/archguard/archguard/commit/37bdffbcab2b4a05ff3297ecf10bba9061f4848d))



## [1.4.5](https://github.com/archguard/archguard/compare/v1.4.4...v1.4.5) (2022-04-03)


### Bug Fixes

* fix workdir issues ([801ee5c](https://github.com/archguard/archguard/commit/801ee5c5ba222a09ddbc1d425bff6b78b600e945))



## [1.4.4](https://github.com/archguard/archguard/compare/v1.4.3...v1.4.4) (2022-04-03)


### Bug Fixes

* fix prefix issue ([dfebe6b](https://github.com/archguard/archguard/commit/dfebe6b502600e024abe1f2af438473fd865500d))



## [1.4.3](https://github.com/archguard/archguard/compare/v1.4.2...v1.4.3) (2022-04-03)


### Bug Fixes

* the ThresholdSuiteService must depends on flywayInitializer ([7344c4d](https://github.com/archguard/archguard/commit/7344c4d406acfa2b23a55ba8ef3bc4cc9c480b5e))


### Reverts

* Revert "build: move metrict bad for creat time" ([d826e34](https://github.com/archguard/archguard/commit/d826e34e85fa0ebcbbb31242f9ddea9487a5aa7d))
* Revert "build: align flyway versions" ([6502582](https://github.com/archguard/archguard/commit/65025825a8ff29a6da524e92689a4b5b5d797c42))
* Revert "build: try to add docker compose for flyway" ([1e6f5ac](https://github.com/archguard/archguard/commit/1e6f5acfdada9c3961f6c3dcb132a439927e3ebb))



## [1.4.2](https://github.com/archguard/archguard/compare/v1.4.1...v1.4.2) (2022-04-02)



## [1.4.1](https://github.com/archguard/archguard/compare/1.4.1...v1.4.1) (2022-04-02)


### Bug Fixes

* add platform for linux/amd64 and fixed [#3](https://github.com/archguard/archguard/issues/3) ([cf033df](https://github.com/archguard/archguard/commit/cf033dfe4894ecf5d1b1d29be70a6d6a5931aaeb))
* **datamap:** fix id issues ([1454376](https://github.com/archguard/archguard/commit/1454376479955566aa7d305c4e056ebd5d5e8574))
* fix removes ([1271633](https://github.com/archguard/archguard/commit/1271633fb47cb842c74768b432922c6ac3b80198))
* fix sql ([398cc30](https://github.com/archguard/archguard/commit/398cc30b0bd1ecf5a6fd89a8823b2a20b2dbf7ad))


### Features

* **data_ref:** add code to db ([2b70e0a](https://github.com/archguard/archguard/commit/2b70e0a087aa9e49a610dfc137467f37830a5ccf))
* **datamap:** init datamap api ([e5810a7](https://github.com/archguard/archguard/commit/e5810a79763ef940ee58e86ce5ab75bc03d9583c))



# [1.4.0](https://github.com/archguard/archguard/compare/v0.1.4...v1.4.0) (2022-03-30)


### Bug Fixes

* add lost db migration ([ca2f581](https://github.com/archguard/archguard/commit/ca2f5810b41d0cdc4667612d584e6327173aa59f))
* add some lost for fields ([0910f39](https://github.com/archguard/archguard/commit/0910f398037fd142efefc6fb9351b9345f86f70e))
* disable type for query ([08dfc61](https://github.com/archguard/archguard/commit/08dfc61fa8c089064bcea971938998c25236f087))
* fix api styles ([8115eb3](https://github.com/archguard/archguard/commit/8115eb3c8c0064b8d246e7778b80295d1135bd55))
* fix controller not /api prefix issue ([b49d93f](https://github.com/archguard/archguard/commit/b49d93fd5afeae3a36567263924b00fca17da59c))
* fix crash issues ([c7e214a](https://github.com/archguard/archguard/commit/c7e214a99483d05f299dc3520d2d3eaf5efd3aa0))
* fix db sql not align db2 issue ([22f9056](https://github.com/archguard/archguard/commit/22f90567cf979e12ecb98a9ee13150a7a5964213))
* fix frontend issue ([1138dc3](https://github.com/archguard/archguard/commit/1138dc392ce0cc0f8fb6f8919acc2c263f9f9ed9))
* fix jclass find one crash issue and set to return first one ([966122c](https://github.com/archguard/archguard/commit/966122c4628e1bb89ee549cd7011eba826037fd6))
* fix lost default config issue for badsmell threshold ([43a1861](https://github.com/archguard/archguard/commit/43a18613c25076883a4b04415c32cf1a2a53481a))
* fix lowcase issue ([2d9296d](https://github.com/archguard/archguard/commit/2d9296dc34e1c31caea8403da1f9ce13e14bb229))
* fix origin code lost branch issue ([feca911](https://github.com/archguard/archguard/commit/feca911c12507ce321d278de840492da28d3a321))
* fix rescan not empty old data issues ([a965d54](https://github.com/archguard/archguard/commit/a965d545bb1858cef28a025f45a165e31f238756))
* fix some refactor typ ([fa1794d](https://github.com/archguard/archguard/commit/fa1794db767a850069ce0f4add1a4f5bd9180e2d))
* fix some refactor typos ([9ba65ca](https://github.com/archguard/archguard/commit/9ba65ca3643c0368c24fb984d46008af21874048))
* fix tests ([ea27e56](https://github.com/archguard/archguard/commit/ea27e56d49c16373f03bea7880625b4e81dd9fd3))
* fix tests not migration ([93fa661](https://github.com/archguard/archguard/commit/93fa6619da24eeef789cc088b9e3d6c5699d94b2))
* fix tets ([ebb9daf](https://github.com/archguard/archguard/commit/ebb9dafc2c5f8eff9d1cd1b20a76fb3bc301ab4e))
* fix threshold issue ([19240a6](https://github.com/archguard/archguard/commit/19240a60051fc6dc34947e2d6d30703d616a5747))
* fix tools versions issues ([15f4ef9](https://github.com/archguard/archguard/commit/15f4ef91783c6fd6001ef151bbfc809395f861ee))
* fix typos for database refactor ([85d1839](https://github.com/archguard/archguard/commit/85d18399b7f2c759d63895b08790f54312ebd846))
* fix unstable file condition issue ([5385b75](https://github.com/archguard/archguard/commit/5385b75b3b97ec165a1b73cff4c7e19f8632cf77))
* fix unused query ([8e3c5c6](https://github.com/archguard/archguard/commit/8e3c5c668aac0e7beec34e94e1935f6b1beff8ae))
* fix url typings ([a72a7dc](https://github.com/archguard/archguard/commit/a72a7dcf4ca1f4fb04edf54340fea2606121ef1e))
* ignore loc for method count ([619c5b5](https://github.com/archguard/archguard/commit/619c5b5061e3ca1b8ccea7bf959a6e3fd31a52ca))
* make scan source code works ([ede2544](https://github.com/archguard/archguard/commit/ede2544b8501395156933c68984341112efb6732))
* remove cached cache ([d4321a7](https://github.com/archguard/archguard/commit/d4321a78c9e2012c6fd0664f4f120efca58e200a))
* remove unused coca badsmell ([abee6b6](https://github.com/archguard/archguard/commit/abee6b6246b7b210d846a1c95cd05716bb26ee5e))
* remove unused typos ([cafc443](https://github.com/archguard/archguard/commit/cafc4434200cd1eceba89370195ef0a15d2f66a7))
* revert fix package start with . issues ([98bbf91](https://github.com/archguard/archguard/commit/98bbf91eb650f9efd7c4d1a0ce45fbc8111cc109))
* set default node to root ([143abd7](https://github.com/archguard/archguard/commit/143abd70619b1e8d7a58389612e3fc042f892f2e))
* update git scanner version ([49ce7ce](https://github.com/archguard/archguard/commit/49ce7ce160d077bd412b0169b152e1fef7d2c8ae))


### Features

* add api naming check ([e29ea07](https://github.com/archguard/archguard/commit/e29ea07930f97292e10d49ce7618ba65b366b355))
* add casing checker ([4212b87](https://github.com/archguard/archguard/commit/4212b87d0536ac9deb79dbd75d45f5a188c982c6))
* add code path for Chapi source code scanner ([2f1cf9c](https://github.com/archguard/archguard/commit/2f1cf9c10cabd15273bda69bb93efbcdfdb1f565))
* add code path in SystemInfo for TypeScript/JavaScript ([5b91a51](https://github.com/archguard/archguard/commit/5b91a51d0644dc35c96fa9f1fce799ac77ee9e51))
* add delete for imd delete for remove system ([7935de9](https://github.com/archguard/archguard/commit/7935de974eb46255d2414f7ee8730b213d9199d3))
* add file length counts ([3088158](https://github.com/archguard/archguard/commit/3088158139df660557aa9fc5c268d3c127516543))
* add handle for module='.' packageName='' ([1c81314](https://github.com/archguard/archguard/commit/1c8131406b98a609d1fe291160c3ab3aba59554c))
* add language count for path change ([4005681](https://github.com/archguard/archguard/commit/4005681ed39e1e52f51fa2340a2ec90c8743dcf2))
* add line count for path ([eb6c9a8](https://github.com/archguard/archguard/commit/eb6c9a86482feac86af89009b484b607ba7455a4))
* add line counts to data ([b621c3c](https://github.com/archguard/archguard/commit/b621c3cd77822872ec32c7b82a21dd52f81e9185))
* add local repo support ([ac25c65](https://github.com/archguard/archguard/commit/ac25c65c39e1c1bdabe07069e452efb629b38291))
* add lost system id to system ([a9aed49](https://github.com/archguard/archguard/commit/a9aed492d938b332c68ec7f2f547d077801bd555))
* add originUrl for debugging ([29efef3](https://github.com/archguard/archguard/commit/29efef3f1022b2e03578148ab3080e74f0a21d90))
* add path change ounct ([38c0c36](https://github.com/archguard/archguard/commit/38c0c360b8f16d8e51326a157b324e9602ddd51a))
* add scan by path support for typescript ([97f00d6](https://github.com/archguard/archguard/commit/97f00d625d4b960e8b573a55b9fc07ef951d02b1))
* add scm path change count ([67770da](https://github.com/archguard/archguard/commit/67770dad209aa51754d6bebc8db633785c6893e9))
* add service language for frontendg ([5efbc84](https://github.com/archguard/archguard/commit/5efbc84e4c6ab272e8f97868fe965e9f5230ef67))
* add service table ([ab68ffc](https://github.com/archguard/archguard/commit/ab68ffcb31722113e141b702b5507a87772d9d22))
* add source target to class deps ([b459848](https://github.com/archguard/archguard/commit/b45984826e36639d22a2c7f4aed8bea6efed9928))
* add suport for source code in multiple language ([ee1db1f](https://github.com/archguard/archguard/commit/ee1db1fa56014ad0e2aa903a7fafb3ec11a0840e))
* add unstable files api ([854013a](https://github.com/archguard/archguard/commit/854013a2966928fba616e5b01a9be26d5c81657b))
* change line counts to sort by languages ([99d678e](https://github.com/archguard/archguard/commit/99d678e5014995ed067fc2682d9cd5e70cac4eda))
* **container:** add query by ids ([10dbd3c](https://github.com/archguard/archguard/commit/10dbd3c7a767545579496df23595fb23618089cf))
* **container:** add support for kotlin rule in regex ([0fb5c73](https://github.com/archguard/archguard/commit/0fb5c7303e998121db6c85078db54d3b91524658))
* **container:** merge demands and resoruces ([c68a297](https://github.com/archguard/archguard/commit/c68a297912d3a0eae380b9fb989c33abeb38aa0a))
* **container:** replace url to real cases ([39c5e44](https://github.com/archguard/archguard/commit/39c5e44c77945303e4b4fe72870b193ca5d8e6ad))
* enable cache for system overview and badsmell overview ([cb3b947](https://github.com/archguard/archguard/commit/cb3b9475ee85a4cd219e9580e081d6dcfa0baf43))
* enable cancel scan ([436b567](https://github.com/archguard/archguard/commit/436b56795f16a87a196942173158b4d938389ad4))
* **git:** align api for hierarchy ([e155124](https://github.com/archguard/archguard/commit/e155124f744777ae15c61bf5fbd2ccbb954831b7))
* init api casing checker ([ef01deb](https://github.com/archguard/archguard/commit/ef01deb1a1411e94c73eb4ad88faf45ed1d7b09b))
* init container services api ([083862c](https://github.com/archguard/archguard/commit/083862ca54cfd58c0a352b83d184661da6aa292c))
* init language overview ([a5569fc](https://github.com/archguard/archguard/commit/a5569fc2d7abad392d929f912434e39fa0ee8eeb))
* make first version flare api ([f1ee7b7](https://github.com/archguard/archguard/commit/f1ee7b76de3e1fe1d1c8b11109f5af905264b14b))
* **multilang:** use jvm as parameter to enable some filter, and prepare for TypeScript or other languages to system ([fb53ea6](https://github.com/archguard/archguard/commit/fb53ea6dbac2f656231958eda9173ee258d46810))
* **services:** refactor demand & resources ([013c74b](https://github.com/archguard/archguard/commit/013c74b6ba076efdd02eabd5e89a708a0facb686))
* **sourcecode:** add soruce target to render to frontend ([46c5132](https://github.com/archguard/archguard/commit/46c51326bc83ecd61b09b667f5c2bfeb67d66cc4))
* **typescript:** add basic scanner and tool for typescript ([4ca42a1](https://github.com/archguard/archguard/commit/4ca42a1a775b62daa593b19d5855e227262a72a0))
* update source code & git scanner version ([d79e336](https://github.com/archguard/archguard/commit/d79e336c603249d132961b91cff942485dc98b88))


### BREAKING CHANGES

* 50% database be refactor, and it had been rename



## [0.1.4](https://github.com/archguard/archguard/compare/v0.1.3...v0.1.4) (2022-03-14)


### Bug Fixes

* add influxDB into docker-compose ([32cbce1](https://github.com/archguard/archguard/commit/32cbce183f6fb5dba434e1a5098fcab7521e9d3f))
* fix kotlin 1.3 to kotlin 1.6 issues ([b343671](https://github.com/archguard/archguard/commit/b34367127cd5f63a250edc8f610ddf59f6f07c56))



## [0.1.3](https://github.com/archguard/archguard/compare/v0.1.2...v0.1.3) (2022-02-22)



## [0.1.2](https://github.com/archguard/archguard/compare/v0.1.1...v0.1.2) (2022-02-21)



## [0.1.1](https://github.com/archguard/archguard/compare/v0.1.0...v0.1.1) (2022-02-20)


### Bug Fixes

* fix cd typo ([fb73b4a](https://github.com/archguard/archguard/commit/fb73b4a06257753db4e2174a773c53c1d802752e))



# [0.1.0](https://github.com/archguard/archguard/compare/a944f0c07d79074390a2375450a4910b3a985967...v0.1.0) (2022-02-20)


### Bug Fixes

* add getAllNormal/getAll to retrive normal/all logic modules ([b83dc6e](https://github.com/archguard/archguard/commit/b83dc6e075f1873ee82f59f9755ab6726dc7a1f1))
* add JClass column access ([04719e2](https://github.com/archguard/archguard/commit/04719e228b53928b8e3dd5bbfdc8cdbc93cbdc1d))
* add jsonIgnore for getNodeId ([ef41791](https://github.com/archguard/archguard/commit/ef41791bd05aaefd89e984159344c222a8a29b9b))
* add persist coupling result after delete logicmodule ([24d8b84](https://github.com/archguard/archguard/commit/24d8b8414038a4e0ef9d6e076b50f908907ffead))
* add projectId for config when create config ([4f61e98](https://github.com/archguard/archguard/commit/4f61e98021921c8672bdc0f19cdb54656b6e9f81))
* add projectId for dependencyRepository and PackageRepository ([4535d2c](https://github.com/archguard/archguard/commit/4535d2c60470a5a96a4b8f7ae71dd91a2f08de1f))
* add projectId for subModulesFromJMethods ([fbfc7dc](https://github.com/archguard/archguard/commit/fbfc7dcf738c22b7d33e62245913b87b55285c2b))
* alyway get class by name and module together ([04d206f](https://github.com/archguard/archguard/commit/04d206f1fe5c384c1c555fecbd52d788c3a5c79b))
* calleePathRegex match callerPath ([9399175](https://github.com/archguard/archguard/commit/939917574fa611338ac3d5e5d7dd388a02376235))
* change displayNode key and value ([2922d88](https://github.com/archguard/archguard/commit/2922d88d82dd1622d279c952d3e35de8b7140757))
* change dockerfile timezone to shanghai and change retrive datetime from db by rs.getObject ([44ddeaf](https://github.com/archguard/archguard/commit/44ddeafb3eefc8dfea28e0303699bcfef6dfa055))
* change regex of getDependenciesBetweenCallerCallee ([bfb111d](https://github.com/archguard/archguard/commit/bfb111da27ddc96274e4a5a4a10eb5157e111d91))
* code tree support ([59eaf03](https://github.com/archguard/archguard/commit/59eaf031af23636bf9929821b32f912dfbd3b032))
* Collection contains no element matching the predicate ([5983b3f](https://github.com/archguard/archguard/commit/5983b3f8d42d4ffea51549e0a1479762038b2ba9))
* conflict ([149fd44](https://github.com/archguard/archguard/commit/149fd4431781aa40b19ac85a7e446d813c81c495))
* db exception ([feb2293](https://github.com/archguard/archguard/commit/feb2293b29e4440f344455906a8f664f3c24527e))
* dubboAnalysisCalleeModules is empty ([9d7e96a](https://github.com/archguard/archguard/commit/9d7e96ae644933947b3ec721e469131fbed100b3))
* exchange fanin fanout ([398951e](https://github.com/archguard/archguard/commit/398951ef18f562100cb4f89e8bcff17cb6fab0e6))
* FeignClientServiceTest assertThat obj ([7d3d31e](https://github.com/archguard/archguard/commit/7d3d31eb8e74191b38b58a2ff70f3c7cff4e1a7a))
* filter a=b in class dependences ([74ce36b](https://github.com/archguard/archguard/commit/74ce36bc192c3955c526cd302bf642d4bd7e492c))
* filter module is null ([cc6fa7d](https://github.com/archguard/archguard/commit/cc6fa7d7b7ee0708a0ac074045c0972ab30c5331))
* fix @Value url with hardcode tmp ([1eac89d](https://github.com/archguard/archguard/commit/1eac89dd3f6a018ece203024b67caf431ab8582d))
* fix abs class bug ([b545cd0](https://github.com/archguard/archguard/commit/b545cd0d94561c5477742582b01b9241d081f322))
* fix argumentTypeList return empty string ([1d30eb6](https://github.com/archguard/archguard/commit/1d30eb658bae06d9a3444378852750f8a3d6511f))
* fix build error ([c88f4dc](https://github.com/archguard/archguard/commit/c88f4dca2f155a8eff606037543fa19442e4296c))
* fix cannot find third party class bug ([ebf6d36](https://github.com/archguard/archguard/commit/ebf6d3644c76c03198b6300f61e4852af3cb5db9))
* fix class coupling data type ([7789f49](https://github.com/archguard/archguard/commit/7789f492a7c0f2749f639d32501ddad83534db29))
* fix class coupling value ([dfdcabf](https://github.com/archguard/archguard/commit/dfdcabf42c70171b0a82a1aa32d8c72dfc41f2e1))
* fix class match multi module ([29a01ce](https://github.com/archguard/archguard/commit/29a01ce71db0ca5331ba43b483cfaed22af07eba))
* fix CodeTree topNode type ([96219f9](https://github.com/archguard/archguard/commit/96219f95aaae81bfb963967d7f5a3f62666124e6))
* fix combining sql strings bug ([e2147a5](https://github.com/archguard/archguard/commit/e2147a5e1a85f70e81994a0073f2f16d23aaa43f))
* fix config wrong project when update ([3edb566](https://github.com/archguard/archguard/commit/3edb566d798c3cf4ac2554c5d31557953eb5e422))
* fix datt issue ([b9d0078](https://github.com/archguard/archguard/commit/b9d007853bf69b05b02893407d622af337bb7e30))
* fix deps issue ([4975f28](https://github.com/archguard/archguard/commit/4975f28c64e4092eeee5c4e4a4a82fff352c9035))
* fix exception ([2a078ff](https://github.com/archguard/archguard/commit/2a078ff478821ee51a224f665e24b2ef20705f4d))
* fix flyway path issue ([1a12852](https://github.com/archguard/archguard/commit/1a128522c391c116752e2916af75256e418eb148))
* fix getIncompleteLogicModuleForJClass ([85f0be2](https://github.com/archguard/archguard/commit/85f0be261f41ff1bb6529dbbb236124ae24afb03))
* fix getJclassBy ([3d01d2d](https://github.com/archguard/archguard/commit/3d01d2db017d10c39bd859aa883db1b3f0af20b1))
* fix import ([68a07c8](https://github.com/archguard/archguard/commit/68a07c8572b8713664ff10f82e211e1a8a6ce2b5))
* fix influxdb title issue ([2bf6d18](https://github.com/archguard/archguard/commit/2bf6d18666417eaea3944243702b66396ad49249))
* fix LogicModuleRepository Cast Error ([f297959](https://github.com/archguard/archguard/commit/f297959df400428f702d9790f074a44c6efb1c01))
* fix migrations issues ([0e2485f](https://github.com/archguard/archguard/commit/0e2485f86da6668465b3ade98b738a292ad81cee))
* fix module coupling data error ([2c1fc1d](https://github.com/archguard/archguard/commit/2c1fc1d50e0c222f1e571c79dc3b21b3426901a8))
* fix origin packages issues ([e762b15](https://github.com/archguard/archguard/commit/e762b1578e27102b178759b229ed657818593726))
* fix project_id && projectId ([dbea087](https://github.com/archguard/archguard/commit/dbea0874d349685d330523492197d55dbe13c793))
* fix resttemplate response void type ([18d8cf6](https://github.com/archguard/archguard/commit/18d8cf6e01c57462301127a12609136921eb9d80))
* fix sql error ([0c8e4ee](https://github.com/archguard/archguard/commit/0c8e4ee72542f097d3237818ea233469a45fcdc0))
* fix sql error in LogicModuleRepository.get ([ada2725](https://github.com/archguard/archguard/commit/ada2725d94351c266a7057df65816d8eccd910d6))
* fix test ([52f83b1](https://github.com/archguard/archguard/commit/52f83b1dba74bb87f9734e2eeb74fc42cb5bf1c1))
* fix test ([9d78173](https://github.com/archguard/archguard/commit/9d781733f12ba3939885f09ebf27d016d4b5ac55))
* fix tests issue ([7bbbf8d](https://github.com/archguard/archguard/commit/7bbbf8d632c16594f8552465e33f3ed0d22e8733))
* fromJMethodDependency ([fee4414](https://github.com/archguard/archguard/commit/fee4414a61b686eec6649725402626a0ef86a8bf))
* JClassRepository location ([0a65e26](https://github.com/archguard/archguard/commit/0a65e26c04f1f90f551ecdac472cee0d294b9833))
* jClassVo not id ([cad312f](https://github.com/archguard/archguard/commit/cad312fdd8990bc41dffc8a667f80ff193360d18))
* jdbi error ([ce54c44](https://github.com/archguard/archguard/commit/ce54c444bd03e48c02111573a4a48a61c0dfb74b))
* JMethodDependency callerMethod ([5f7554f](https://github.com/archguard/archguard/commit/5f7554f382f6232a9e86764c9817e69be9d92aca))
* LogicModule add & remove ([b6eb66c](https://github.com/archguard/archguard/commit/b6eb66c0bca64bbaa86781da4d3c706ca9b935bb))
* matchPath with findFeignClientMethodDependencies ([bedcaae](https://github.com/archguard/archguard/commit/bedcaae1113993b1021ac1b03f9e657faed1af07))
* modify table JAnnotationValue value to text ([e69e9fe](https://github.com/archguard/archguard/commit/e69e9febe62fd599cdbafdcfe79113b86e29fcee))
* NaN issue when no dependence found ([fc90f7c](https://github.com/archguard/archguard/commit/fc90f7cc763640426e9a8e00b76a74ab167e4e1d))
* open eureka in local ([cbc7f78](https://github.com/archguard/archguard/commit/cbc7f7857ec0e94dfb9e441b389e1127abe80fb8))
* regex of matchPath ([4d916d9](https://github.com/archguard/archguard/commit/4d916d9b4d810816205186776d11fb9c24ef9d13))
* remove cycle relationship caused by call itself ([66fe0ad](https://github.com/archguard/archguard/commit/66fe0ad22c307a926cf633bf4e394e4d429a425c))
* remove logic module'members whose module is outside ([b2d1e7c](https://github.com/archguard/archguard/commit/b2d1e7c38a25d90eed11148cff078193b732c949))
* remove wrong return ([d41fbf7](https://github.com/archguard/archguard/commit/d41fbf7a1dfe1f046ceb81eefc7aafcddc58d85e))
* rename wrong log ([8e1ef20](https://github.com/archguard/archguard/commit/8e1ef20d59b809f7e9f5448a43c00c034f9f51dc))
* returnn abc with class set count ([72e3ec6](https://github.com/archguard/archguard/commit/72e3ec66bf03a343f8510ab9f64642cad10e7e70))
* rollback graphService use getAll ([a07f143](https://github.com/archguard/archguard/commit/a07f14312758bcee805807ef5f7a66c049de6198))
* skip test in buildSource ([fe131cb](https://github.com/archguard/archguard/commit/fe131cb26cbf9d143792b54591dde4c3dd3815f7))
* startWith match the longest one ([0a3353b](https://github.com/archguard/archguard/commit/0a3353b5bb764a12cba13d5376748fe24e79c093))
* support one caller/callee class/interface with multi-implement module denpendency relationship ([1ff9bb1](https://github.com/archguard/archguard/commit/1ff9bb159dce38a061b020d9d5171872914a5303))
* support one interface with multi-implement module denpendency relationship ([340083e](https://github.com/archguard/archguard/commit/340083e9812ecb6efe38182df4309effadcd7422))
* temp fix coupling legacy service ([69b7dc0](https://github.com/archguard/archguard/commit/69b7dc0d99a17b7c8725da6c305ec0efb2d1d1c8))
* temp fix coupling legacy service ([b1f6d76](https://github.com/archguard/archguard/commit/b1f6d76edeb5dc64b59995cdb849c418976aa138))
* url of queryPluginTypes ([ef0bc10](https://github.com/archguard/archguard/commit/ef0bc10f08d6457784871bda1d3633169c7fbb2b))
* use influxdb.url in application-local.properties ([9c37c04](https://github.com/archguard/archguard/commit/9c37c04120c1855e8c4ca6592a5d9cca35d09089))
* use startWith match when not fullmatch ([b6bf40b](https://github.com/archguard/archguard/commit/b6bf40b3797e01b7fa70377ad1940efaa543223b))
* xxx-a.b.cccc defineTableTemplate should not match xxx-a.b.c ([4856170](https://github.com/archguard/archguard/commit/4856170b0b657253886d4b0361414814a8e6b4a0))


### Features

* add  to 3306 ([bd40837](https://github.com/archguard/archguard/commit/bd40837b801a493f1f7621af822ea86a447e4a3f))
* add abstract class LogicComponent to init Composite Pattern ([6d2fa18](https://github.com/archguard/archguard/commit/6d2fa183dd2cfc69b8f9466953211f90921dd9d4))
* add abstract metrics ([0d3d85f](https://github.com/archguard/archguard/commit/0d3d85f0181744a6d58537cdc6d17688f3d28dc2))
* add AbstractAnalysisService ([c229908](https://github.com/archguard/archguard/commit/c229908e3bda440337d1fd01e93eaa355d994f42))
* add analysis ([2d7c2ea](https://github.com/archguard/archguard/commit/2d7c2ea9ce3d65d300f8a6755e8bca4aa9df463f))
* add analysis ([646969d](https://github.com/archguard/archguard/commit/646969de63a3876af6074699de26566f853b1031))
* add analysis ([f3f356b](https://github.com/archguard/archguard/commit/f3f356b61677fad778ceba96846493ec86425249))
* add analyzeRequestClass and Test ([2a54c2a](https://github.com/archguard/archguard/commit/2a54c2ac9767d0fc3ac51bd446f0ec79cf69ef89))
* add api for updating config by type ([4bb0a83](https://github.com/archguard/archguard/commit/4bb0a83558c12cd3f72fcc4206373573e9018793))
* add api to get module coupling by by class ([a3d6ea2](https://github.com/archguard/archguard/commit/a3d6ea210df7bcc27df35da614002887341aadcd))
* add api to get package dependence group by modules ([c4741af](https://github.com/archguard/archguard/commit/c4741af50942ad9814ad705f924ffb831f08644b))
* add apis for quality gate profile ([bc0b239](https://github.com/archguard/archguard/commit/bc0b23915ac5f35d7e9b498af250f3412affd307))
* add autoDefineLogicModuleWithInterface ([e998196](https://github.com/archguard/archguard/commit/e99819626e22d4807d96b173ef4e01e3be3c205c))
* add badsmell ([e57d3a3](https://github.com/archguard/archguard/commit/e57d3a3edc764981cc5ae24146f4cc59e72fdb10))
* add badsmell query ([29c325f](https://github.com/archguard/archguard/commit/29c325f0ce1b71b2a652682c95a2441366d646d4))
* add badsmell service for scan ([1a77c5d](https://github.com/archguard/archguard/commit/1a77c5da0f2ebab53fab2d106860589d56d194bb))
* add bs and tbs log ([ece9dd1](https://github.com/archguard/archguard/commit/ece9dd109ccd744a393e7f61bc83ad39af8bd6b3))
* add build tool in compile project ([b5dc240](https://github.com/archguard/archguard/commit/b5dc240e7f1eae9ddebd0056ccfb261c32afc40d))
* add build tool in scan context ([4ff2e13](https://github.com/archguard/archguard/commit/4ff2e13b175b1477a7b6d0a95ac35a00f42c9b38))
* add buildTool enum ([92ab81d](https://github.com/archguard/archguard/commit/92ab81dea2b2967c9223cb9b26cce6788cb7c30e))
* add calculate coupling request ([4aaa115](https://github.com/archguard/archguard/commit/4aaa1157a0bfbcc246f4e921194046ea91e35006))
* add calculate metrics from class package module ([e72de81](https://github.com/archguard/archguard/commit/e72de81011da8a46811a7caa3109885dc26f9b96))
* add calculate metrics when update and delete logic module ([c8b4594](https://github.com/archguard/archguard/commit/c8b4594f1dd1e100808140bb0b2733768f64c997))
* add calculateAllModuleCoupling api ([ecb4a25](https://github.com/archguard/archguard/commit/ecb4a25e34b5ffb81af92d8935bea4eb9d0ed876))
* add ChangeImpactQualityReport ([c26528d](https://github.com/archguard/archguard/commit/c26528d27a4c5df0f716e7ced6202432fa1d7518))
* add changeImpactReportDetail ([91fd776](https://github.com/archguard/archguard/commit/91fd776b3aa548edb293c4ef4de67f11e2bbff00))
* add class -> package -> module report ([45b10c1](https://github.com/archguard/archguard/commit/45b10c19e168c8d91aad5b5aaf93176330673e71))
* add class coupling table ([354d85c](https://github.com/archguard/archguard/commit/354d85c7ec3405b32a9aea2254520083a983eeb5))
* add class invoke api ([6f37944](https://github.com/archguard/archguard/commit/6f37944257890625f8b3c74a866556f9d5138a46))
* add class metric infludb ([445138c](https://github.com/archguard/archguard/commit/445138c34dc506fa4e2f1c6d503228c96b552adb))
* add class metric persist ([ac1db98](https://github.com/archguard/archguard/commit/ac1db9823d44df506bcc2d9cc7267ba0a76175ae))
* add class_metrics table ([9d1a86b](https://github.com/archguard/archguard/commit/9d1a86b0ca8f3a2a3bc20c90caef8f8c7272674d))
* add ClassCouplingDtoForWriteInfluxDB.kt ([aed7e96](https://github.com/archguard/archguard/commit/aed7e96a89c050f850dfb326fe919abc5a94c996))
* add ClassCouplingDtoListForWriteInfluxDB.kt ([ceb7248](https://github.com/archguard/archguard/commit/ceb7248b9cb5a602b70804c9e6f21018df1fc546))
* add ClassMetrics DepthOfInheritance ([faf6b0f](https://github.com/archguard/archguard/commit/faf6b0f7ed87c180045df2e2461545ad791c0637))
* add coca scanner ([42b4805](https://github.com/archguard/archguard/commit/42b48051dcc7048a045a1ed17afb2068a349916f))
* add coca scanner ([d2dc3ea](https://github.com/archguard/archguard/commit/d2dc3ea31b5ae02896bc35757630f3aac2fef40d))
* add code scanner ([a944f0c](https://github.com/archguard/archguard/commit/a944f0c07d79074390a2375450a4910b3a985967))
* add column name ([5dac26d](https://github.com/archguard/archguard/commit/5dac26decd7ab8bfc97d3bc31ec057106de7cca8))
* add comment ([1be4914](https://github.com/archguard/archguard/commit/1be491409562891ddc2cd618570675cfed280e02))
* add comment and level ([b02878b](https://github.com/archguard/archguard/commit/b02878b3d67c4071e13b0348d08a65436740b4f5))
* add configure and logic module add projct id ([48a8de3](https://github.com/archguard/archguard/commit/48a8de3246bce675596ad9ddc197d515688b2258))
* add connection timeout ([38b18e9](https://github.com/archguard/archguard/commit/38b18e96b9e9020eba83f036a1f888cf5d311027))
* add connection validation ([00e5dae](https://github.com/archguard/archguard/commit/00e5dae37882204ee5eac3ae629f7acc28e24866))
* add constructor for commit log ([c7ffc41](https://github.com/archguard/archguard/commit/c7ffc4167cf39fa78c6792b873aec7ae2aab1fb7))
* add controller ([9f81ad4](https://github.com/archguard/archguard/commit/9f81ad40dc9b4d9db2a3e2a2debd71d9f72c9cd3))
* add controller ([717ccd2](https://github.com/archguard/archguard/commit/717ccd2ee88de3ae62be9c9efefeb77f6d089b89))
* add create/update/delete api for config ([5302b3f](https://github.com/archguard/archguard/commit/5302b3f7a6282f459e547751b6bfe352d9e1e8d6))
* add curd logicModule and dependence ([08b8f80](https://github.com/archguard/archguard/commit/08b8f80d7a485d8fbe9d3f8641d1beab7d180a21))
* add delete ClassCoupling fun ([a4f6bd2](https://github.com/archguard/archguard/commit/a4f6bd23a45f191affee55b468dabc3b262fe6fc))
* add DependencyAnalysisHelper ([a413e86](https://github.com/archguard/archguard/commit/a413e86ad286d327d328df6ef2a380f9785a7973))
* add design ([36e0908](https://github.com/archguard/archguard/commit/36e0908c74c81ad1b5280c2ee02a26e1f3d46f1c))
* add designite Java bad smell ([fbf94b2](https://github.com/archguard/archguard/commit/fbf94b2f7632ff5fbd4edc5c6d4a656f715a1cbb))
* add dfms metric ([709557f](https://github.com/archguard/archguard/commit/709557f0d4401b4749cdbcc3cc85e1f492d1682a))
* add dfs calculate connectivityCount ([7e74ebd](https://github.com/archguard/archguard/commit/7e74ebd773a1b21f6d154975ddfb8b2baf1bca9a))
* add dit controller ([79cd179](https://github.com/archguard/archguard/commit/79cd179bc959ef30e903863387a943e76524e761))
* add domain exception and add delete project info api ([29f43a8](https://github.com/archguard/archguard/commit/29f43a8335e30f4127720078331d420da1ddcb11))
* add dubbo related table ([34dbbee](https://github.com/archguard/archguard/commit/34dbbee47b4f8371b5dd205f9c58b0aa701e12ce))
* add DubboPluginTest ([7c86d96](https://github.com/archguard/archguard/commit/7c86d9658d21ee1f842b83f44fb2cf78280b7b9a))
* add DubboPluginTest ([a61553a](https://github.com/archguard/archguard/commit/a61553aeb058f306080f4e76d62e6e0ea4e7822a))
* add DubboWithXmlPlugin ([80afe32](https://github.com/archguard/archguard/commit/80afe32155cc920500e5d7a2a5c9a8a884e578b5))
* add evaluation ([9829742](https://github.com/archguard/archguard/commit/98297428bb5a51228427ee17e15bbb9df48e9a87))
* add evaluation report ([cfc6e63](https://github.com/archguard/archguard/commit/cfc6e636d66a661284b124c9474b5ee56c824e95))
* add evaluation test ([809cd15](https://github.com/archguard/archguard/commit/809cd158cd1b9d4e006b100bdbe61a0902e2a562))
* add Factory method for LogicModule ([7c42c0d](https://github.com/archguard/archguard/commit/7c42c0da6f3910f8c50ca349e738bc8fc3ab9cd5))
* add fanin and fanout ([d117806](https://github.com/archguard/archguard/commit/d117806b064bf735b7bab7738296763220c2a6ad))
* add FeignClientGraphServiceImpl and test ([5ea8590](https://github.com/archguard/archguard/commit/5ea859085d0ea497fce7f240f5d55f1f6d3f5d9e))
* add FeignClientPlugin and Test ([2328874](https://github.com/archguard/archguard/commit/2328874272bdf7a66c5cf95a94b6e83ef76bef4f))
* add FeignClientService ([f4da28b](https://github.com/archguard/archguard/commit/f4da28b6a784e5d7e197d00719d3cc9fd44790c1))
* add filter ([af52594](https://github.com/archguard/archguard/commit/af52594be30cde6dd4dfd2d17f18296ce2905c95))
* add fixed thread pool to run scan ([231e770](https://github.com/archguard/archguard/commit/231e770101e68a86be035138f7948607476ba987))
* add fixme for doBuildCallees because cost too much time to calculate ([5cf43e4](https://github.com/archguard/archguard/commit/5cf43e46698d37c5945aff496bc5032f02c8951e))
* add fixMethodDependencies to dubboPlugin ([c1707d3](https://github.com/archguard/archguard/commit/c1707d3a65bd89ac5f499bd7ed8a0088ff1a57c3))
* add flyway ([d76a31e](https://github.com/archguard/archguard/commit/d76a31e12acc26bca4532e7d6dbe3dfa03f8ded2))
* add flyway local config file ([ef9e484](https://github.com/archguard/archguard/commit/ef9e48480d0f424f9709b323f5098b0b3dc521a3))
* add generate ([71eb6e5](https://github.com/archguard/archguard/commit/71eb6e5096f03f5dd48b614b926bbfccc4535d94))
* add get all baseModule ([ed11cef](https://github.com/archguard/archguard/commit/ed11cefb78a850f63785bd29aa7cbb436237dcc0))
* add get all ClassCoupling ([c17bb88](https://github.com/archguard/archguard/commit/c17bb884f6b79bb3e14c5db2b4f672460992bb0f))
* add get all configure api ([3dd5630](https://github.com/archguard/archguard/commit/3dd5630b8a6800fc3165afe6b06b9730f839b122))
* add get class dependency api ([1d18a64](https://github.com/archguard/archguard/commit/1d18a647d00134248edfe24f16fbe01fb362ce46))
* add get class method callees api ([124465b](https://github.com/archguard/archguard/commit/124465bcd02713d4011f5d42c70e4f877622b754))
* add get method callees function ([cb8b2e2](https://github.com/archguard/archguard/commit/cb8b2e23cc9b57139e415d77a0205fdd565a2489))
* add get name ([5b3ea8b](https://github.com/archguard/archguard/commit/5b3ea8b8fa58772460124b861e57bfdb052bb6cb))
* add get only module metrics api ([cd2d5da](https://github.com/archguard/archguard/commit/cd2d5da992189b35f9698db583dae79b8ca03bf1))
* add getPackageName to JClassVO.kt ([af513ae](https://github.com/archguard/archguard/commit/af513aec84c0ceabcb5179e5f714fb91b10406cc))
* add git analyze ([36690d9](https://github.com/archguard/archguard/commit/36690d97c3d7c96aa102eaa586c83566a67bcf2e))
* add git commit analysis controller ([e6a6091](https://github.com/archguard/archguard/commit/e6a6091e59e498b62754ab9bc83f1d2a507cdeef))
* add git source scanner ([5792044](https://github.com/archguard/archguard/commit/5792044f20607b257b2b05a9da570294f9477c0f))
* add git tables ([540f7a6](https://github.com/archguard/archguard/commit/540f7a6d8cb733fcb1802e2edffb73e861499377))
* add GraphService to controller ([04cf2e5](https://github.com/archguard/archguard/commit/04cf2e524350525a4102f521a851ca071d80c025))
* add GraphServiceTest ([967c4c1](https://github.com/archguard/archguard/commit/967c4c1b10e178a744324fcedbb08671ed98f458))
* add hideOrShow for ClassMethodCalleesService ([3c2422e](https://github.com/archguard/archguard/commit/3c2422ef0d2d9bf43548ab68dff932cd42733c87))
* add hideOrShow for MethodService ([baf4a2d](https://github.com/archguard/archguard/commit/baf4a2d69c2415f482f188270d22afc6dc21a5f9))
* add hideOrShow node in ClassDependence ([53bc4aa](https://github.com/archguard/archguard/commit/53bc4aaa9c20376f5cb3da403038f3c071b41927))
* add host ([cabd5e4](https://github.com/archguard/archguard/commit/cabd5e47052480c36f660427f2c8f4bd2c4965e1))
* add HttpRequestService and Test ([c343772](https://github.com/archguard/archguard/commit/c343772abe9190aacdf6ae6619c325298948f85e))
* add index for class_coupling ([97328ea](https://github.com/archguard/archguard/commit/97328eaca0a0f0a31231357496e499f51733dfcd))
* add insert to sql ([7cdd803](https://github.com/archguard/archguard/commit/7cdd8032739db0d16778f0f808fc70bb75847ee5))
* add integreation test for ([cfc59da](https://github.com/archguard/archguard/commit/cfc59da53173b285f785068c4df2219fcd1c3fb5))
* add integreation test with @Sql ([a962fb4](https://github.com/archguard/archguard/commit/a962fb4133c8449be174e0624ea5f6384a407c3a))
* add invokes plsql ([a6faaa8](https://github.com/archguard/archguard/commit/a6faaa87781e9a4d2fec6ef574fadb68549da013))
* add item Name ([6a5701f](https://github.com/archguard/archguard/commit/6a5701f9fb2ddf531ccb4ff123a8be77e007a92f))
* add item_name ([3665958](https://github.com/archguard/archguard/commit/366595875fac2af10a9f3a9179e09765450dc3e1))
* add JAnnotationRepository ([991c9d2](https://github.com/archguard/archguard/commit/991c9d25ea8c936fccc87081f8c9dc188639537d))
* add java dependency scanner ([86618bc](https://github.com/archguard/archguard/commit/86618bc32afef6233d7dbe2148d800886ed4ddd0))
* add Jclass ClazzType: interface or others, remove useless equals and hashcode ([6f755e7](https://github.com/archguard/archguard/commit/6f755e71245dd37535b09e3e3a349ff2e753afa0))
* add JClassVO ([73c2f8c](https://github.com/archguard/archguard/commit/73c2f8cdecb2827d0e17ff9758376abe650e4742))
* add jdbi ([0257fad](https://github.com/archguard/archguard/commit/0257fad76a11bb4eaccc04e5e089ed5772fda4b4))
* add JField for Class and method ([be47855](https://github.com/archguard/archguard/commit/be478556c9263a17bb1a05ae351b92a5e74a76f5))
* add jmethod table column argsmenttypes ([f26bf7e](https://github.com/archguard/archguard/commit/f26bf7e250f5216e9c7311a9ccf23c72307418b7))
* add JsonUtils ([8d181d2](https://github.com/archguard/archguard/commit/8d181d216e97f4c2b505fad8ae1c13c5f229a173))
* add latestModuleTestCoverage ([eeee9c0](https://github.com/archguard/archguard/commit/eeee9c0800b103ef8c99e9a8e77600ce2403f899))
* add latestTestCoverage ([fdd9187](https://github.com/archguard/archguard/commit/fdd91877071fbe8c0a85cf19193492ccd9ec53ba))
* add latestUselessTest ([0ad4241](https://github.com/archguard/archguard/commit/0ad4241bcb2316e8b8d5d274bf8082f94b5775f6))
* add lcom4service ([1ce5ff8](https://github.com/archguard/archguard/commit/1ce5ff8108b3ad42d7cafe8876801500290f7610))
* add level ([addb9fb](https://github.com/archguard/archguard/commit/addb9fb0a3607b6dfa711c45789564ac64fc8588))
* add lg_members for logic_module ([dd90c15](https://github.com/archguard/archguard/commit/dd90c1524d0c2aa5de295c550255585a1aeb21a3))
* add local ([8228fb5](https://github.com/archguard/archguard/commit/8228fb5ef1b138d210b0d442c46949da745c8178))
* add log ([47e5507](https://github.com/archguard/archguard/commit/47e5507ee017bed3eda69ef989cfc70c235d0565))
* add log to debug ([250dd13](https://github.com/archguard/archguard/commit/250dd134582b81178704ccd67ea53c1868163bd6))
* add LogicModule class and table show status ([68d6abb](https://github.com/archguard/archguard/commit/68d6abb2509818f456005eb605176bd9347e16b8))
* add LogicModuleGraph and Test ([5713639](https://github.com/archguard/archguard/commit/5713639036973d5f892a9d7ce81ea06a25c1a79b))
* add mapModuleDependencyToServiceDependency ([c2d25ad](https://github.com/archguard/archguard/commit/c2d25ad611dc942765897321b717c199e4f03d7d))
* add mem to 2g ([f8af2e9](https://github.com/archguard/archguard/commit/f8af2e98bde982266ddb50bbc7b7f86fa069edf8))
* add method caller callee invoke apis ([4c46f7e](https://github.com/archguard/archguard/commit/4c46f7ef14d9c581ff1a1864676bc54f2da3b79e))
* add MethodField relation ([463f8c8](https://github.com/archguard/archguard/commit/463f8c8ee6e3bdbffcb441da6dad00c2867568a4))
* add MetricController ([9c72fd2](https://github.com/archguard/archguard/commit/9c72fd22300239c24cbe5266142f3241f858e2a0))
* add MetricPersistService ([7ed3bde](https://github.com/archguard/archguard/commit/7ed3bde01abba5e520a049bfdbb1813294461d7f))
* add metrics module package class table ([84fde2f](https://github.com/archguard/archguard/commit/84fde2f8d132cd3a74ff3fb0622037ca47e4f9a1))
* add metrics module package class table ([d247d4e](https://github.com/archguard/archguard/commit/d247d4e4c60f118943621b47cc280fc9cf9df896))
* add metrics service test ([cd62c73](https://github.com/archguard/archguard/commit/cd62c73ef8719b0d015eef47ac99bbbf52a2f7a0))
* add module coupling api for logic module ([eae5541](https://github.com/archguard/archguard/commit/eae5541ae322026a873b5aae2b1df2e54eadb53c))
* add ModuleCouplingQualityReport ([c0139ec](https://github.com/archguard/archguard/commit/c0139ec619caaab937c80086e688fbdbe1bd54f8))
* add new Graph ([ef34d26](https://github.com/archguard/archguard/commit/ef34d2669cdc3d1270877bc549a3ba1b5bef113d))
* add new Graph with Generics ([1f0a92a](https://github.com/archguard/archguard/commit/1f0a92adaee096fd6ef61baf84d9fee88e98cae7))
* add noc for class ([27db420](https://github.com/archguard/archguard/commit/27db42009040d3a672021702799a2775cccbe7ed))
* add NoModuleFoundException ([82c1e85](https://github.com/archguard/archguard/commit/82c1e8584eaaebcf0756f3a806c45f94f70c3014))
* add Overview ([e44c619](https://github.com/archguard/archguard/commit/e44c6190dda840aa78f5617bd4a7a901e8b1b62d))
* add path to FeignClientArg ([d9123e7](https://github.com/archguard/archguard/commit/d9123e79326cc7c8ceac751e3bea7a07adc68cc6))
* add Plugin and PluginManager ([bb86210](https://github.com/archguard/archguard/commit/bb8621063efbcfb5bb5dea6f82f5676b2c52d944))
* add PluginConfig ([ee6ef20](https://github.com/archguard/archguard/commit/ee6ef207838e53401315de2c437abb5f3347c89d))
* add processBuilder wait for signal ([616d902](https://github.com/archguard/archguard/commit/616d9024b51f756269c0d13f10f1acc85c4464ea))
* add quality gate profile table ([2600472](https://github.com/archguard/archguard/commit/26004721f2354595c7628e0aeb286f2e1dd0b9a4))
* add query by id ([ab36231](https://github.com/archguard/archguard/commit/ab362318b6ef2e962161bf6586b9b5eb39553779))
* add query detail ([52fccd4](https://github.com/archguard/archguard/commit/52fccd4dcdde566e91909bc4f42f585b5caa6d88))
* add query fanin and fanout ([a47dcf7](https://github.com/archguard/archguard/commit/a47dcf79b91007bb789f32c04a6c9c979caaa034))
* add query module metrics ([1bf728d](https://github.com/archguard/archguard/commit/1bf728df15b20497c616fe0cde525facef21fadb))
* add queryPluginTypes api ([710d505](https://github.com/archguard/archguard/commit/710d505ee547229e3d3a6ebe6347106f78d57632))
* add README.md ([e44ed85](https://github.com/archguard/archguard/commit/e44ed8510e559d34ec4ddc15703b0bc245b0ab7d))
* add README.md ([155df5f](https://github.com/archguard/archguard/commit/155df5f09ad53930c7359692643ded2b5806c18a))
* add repo metrics test ([5df81d8](https://github.com/archguard/archguard/commit/5df81d84724d98f9b3af8d8552b0c88366ca3c88))
* add report detail ([5d208fa](https://github.com/archguard/archguard/commit/5d208fa10911fb82d3bfe6eb0b91163194a9bce6))
* add returntype and argumenttypes for JMethod ([f9fa8f4](https://github.com/archguard/archguard/commit/f9fa8f4ee39176e4601bcb0d0ce3d0d1a819eafb))
* add returnType and argumentTypes to JMethodVO ([726aa26](https://github.com/archguard/archguard/commit/726aa26d2006ec6d34639815dd236c1d0f2c63fd))
* add row mapper ([1d70866](https://github.com/archguard/archguard/commit/1d70866379ca837a8660ab5acebd717f879408d6))
* add scan ([d444f99](https://github.com/archguard/archguard/commit/d444f99613109574a60bd35d467c29cda7d9609e))
* add scanner ([76a9a27](https://github.com/archguard/archguard/commit/76a9a2795428304c1e9d14805921b5104d29cf25))
* add scanner ([0b6819c](https://github.com/archguard/archguard/commit/0b6819c36ded05cc1e6deefa5fb288113a00dbff))
* add scanner ([9ab8130](https://github.com/archguard/archguard/commit/9ab813015cb65eece5d3a178514102a78372741d))
* add scanners ([7cdcb41](https://github.com/archguard/archguard/commit/7cdcb4113a3a44994ebb33542fcb696b07bdbfc1))
* add scatter commit ([ace0e06](https://github.com/archguard/archguard/commit/ace0e061df8c02e393719f0c27a681aea511a6e2))
* add some scanners ([ca6f9b2](https://github.com/archguard/archguard/commit/ca6f9b22684f2bea15884f9ec0093a7da5213c7a))
* add sql ([4db9be1](https://github.com/archguard/archguard/commit/4db9be1a71af83fc7bfb309a0514e6f451d72900))
* add sql dependency ([b844852](https://github.com/archguard/archguard/commit/b84485265db9695db491a98bf834359690619fe7))
* add sql table ([5b9dd0d](https://github.com/archguard/archguard/commit/5b9dd0dbff852c8fd0a2280995f570f10e7823ce))
* add status query ([8a0bd9c](https://github.com/archguard/archguard/commit/8a0bd9ca03ae7fd4392d1f7c9378fce0ff94d912))
* add svn support ([77b6170](https://github.com/archguard/archguard/commit/77b617084201c032e9c805c54f51a1c13a00a196))
* add table JAnnotation and JAnnotationValue ([ad9b56c](https://github.com/archguard/archguard/commit/ad9b56c5df39505657d2b413285751503e37229c))
* add test ([048cea3](https://github.com/archguard/archguard/commit/048cea32268003230d70c098d0f14a243304764d))
* add test ([695ad84](https://github.com/archguard/archguard/commit/695ad84f8267741cf6ad4bd3b0f166521fc4cefc))
* add test bad smell ([d23b958](https://github.com/archguard/archguard/commit/d23b958dfe8886a00282bcda42a5191b50a0f625))
* add test bad smell ([a3aaac6](https://github.com/archguard/archguard/commit/a3aaac659bde583fb2b7881e1f8445e7b6ae0959))
* add test bad smell count ([d335b63](https://github.com/archguard/archguard/commit/d335b63cdc59cf3ef6662ab7cbd5371e4a976ff5))
* add test for bad smell ([43f52ef](https://github.com/archguard/archguard/commit/43f52eff897b4d0c606ead2845787b9cad558553))
* add test for dubbo xml analysis ([a8bb9d8](https://github.com/archguard/archguard/commit/a8bb9d87e46668ad3253b7201242cf3c481df6ee))
* add test of logic module ([6c7c5e2](https://github.com/archguard/archguard/commit/6c7c5e23bd41bfae1f005aadd31d75b55f1361ac))
* add to 1G ([2f06bb8](https://github.com/archguard/archguard/commit/2f06bb8c394f796e32363e8cec790ff9368052cf))
* add toString for LogicModule, JClass, SubModule ([ba7964d](https://github.com/archguard/archguard/commit/ba7964d8829ac0304158b4c3f2d8d7846fa722a1))
* add total test ([1470566](https://github.com/archguard/archguard/commit/147056611121e91a2b8e9eda17aed1a0f0b83e01))
* add useless percent ([f247452](https://github.com/archguard/archguard/commit/f247452468fa55c7b0cd63398a76c35e744efc94))
* auto define logic module with specific interface ([2b832b0](https://github.com/archguard/archguard/commit/2b832b05e6774e538caf178f2e8c5fe3c95b7781))
* change classType to List ([69a3e88](https://github.com/archguard/archguard/commit/69a3e88ed4cd6cd49ca2556b5d56dd1a4e64b6da))
* change entity name to 200 varchar ([0e8f5d3](https://github.com/archguard/archguard/commit/0e8f5d3d86e1c12996fd62dc6a4ca1bf5d406124))
* change getAll to getAllClassDependencies ([e920204](https://github.com/archguard/archguard/commit/e920204782ea005a2445ccb850bdff513c839c02))
* change package-list api from get to post ([86d6cdc](https://github.com/archguard/archguard/commit/86d6cdceb275bc997348fbbd51eddaa86e586947))
* codeTree support top class file ([af2afe7](https://github.com/archguard/archguard/commit/af2afe788eb7616551a3ebb7a2b14f74850d71c3))
* combine dependence_module and package and remove useless folder ([4204a98](https://github.com/archguard/archguard/commit/4204a981a8513bc7cab173a77dcc720f6a54310b))
* correct  designite java tool ([59c465f](https://github.com/archguard/archguard/commit/59c465fa3c59ea79be146a326d699d99f75e5615))
* correct bad smell ([6ad6a4a](https://github.com/archguard/archguard/commit/6ad6a4aeaeb626d66cbd0d6fc800fb307c069b33))
* correct coca tool url ([7a48db5](https://github.com/archguard/archguard/commit/7a48db5a35cc5198f7c6340987db26bf84af1d02))
* correct compile ([b7b361a](https://github.com/archguard/archguard/commit/b7b361aa153402c7c3033c34bca2a458414fe9d0))
* correct conflict ([fe5c0e6](https://github.com/archguard/archguard/commit/fe5c0e68e1c228f45e949a5f153b7b8a0ab6e225))
* correct content ([a4a55c4](https://github.com/archguard/archguard/commit/a4a55c412890bc7c422c293319fd189af027d8a3))
* correct dburl ([b242333](https://github.com/archguard/archguard/commit/b24233366f6a47c4ed3afef1433611539c11a571))
* correct DELETE_PROCEDURE ([c23f7aa](https://github.com/archguard/archguard/commit/c23f7aa02de76ab16d3517333604992a78f415d6))
* correct enum ([529e5e9](https://github.com/archguard/archguard/commit/529e5e91685f843ec39a869975ca672d88584c23))
* correct evaluate ([ffa615e](https://github.com/archguard/archguard/commit/ffa615e498aff1e329526312d6dcf51d77036d25))
* correct evaluation ([c32de0b](https://github.com/archguard/archguard/commit/c32de0b67e875a1f4d526897aa3689541937d74c))
* correct git scanner tool ([ba738e6](https://github.com/archguard/archguard/commit/ba738e6098edfd6684f403cbfac151f1321fff47))
* correct hubservice ([66b14aa](https://github.com/archguard/archguard/commit/66b14aaa54e64532ae847d83257c684d47c6a8f5))
* correct improvements ([0d575f4](https://github.com/archguard/archguard/commit/0d575f4842f4dc5a69034c6e9d7454f50062ee0d))
* correct jacoco ([9183752](https://github.com/archguard/archguard/commit/9183752fe36ca275862d9b4f3e180d34336ab42e))
* correct java byte code tool ([8bb22e7](https://github.com/archguard/archguard/commit/8bb22e7ae30503d6bad39c0db05505df8d5b9a8b))
* correct java dependency scanner ([ef86ee8](https://github.com/archguard/archguard/commit/ef86ee8fb2fdd53811f58147c24df75ebf720b89))
* correct kotlin ([085ac95](https://github.com/archguard/archguard/commit/085ac95541dc92ee2651c8526947ffc507afb00e))
* correct map ([8c129a9](https://github.com/archguard/archguard/commit/8c129a9eb1c71e36d32c43cd07a39a5dc52fa1c5))
* correct module coupling report ([ac0e674](https://github.com/archguard/archguard/commit/ac0e674e7f4242b0348e7be12d71a94ff6cae1f2))
* correct project info ([512efae](https://github.com/archguard/archguard/commit/512efaedb486d89db741c7cfdf08f8a3abeef870))
* correct report ([6d7ff89](https://github.com/archguard/archguard/commit/6d7ff8974c6619567eb5085aa9b9ae1949474e5e))
* correct report ([1b32c07](https://github.com/archguard/archguard/commit/1b32c077776cc243a96d6e613cad431ce0fe24b9))
* correct rule ([24f9cda](https://github.com/archguard/archguard/commit/24f9cda83916f6134a057059728f59747c64e874))
* correct scan git url ([1933955](https://github.com/archguard/archguard/commit/19339558220fc9251db4b6a1fa2a85d9a16c448d))
* correct scanner ([3cb9f11](https://github.com/archguard/archguard/commit/3cb9f11702fb4dc9a437fc2bd3e07f24ab60b460))
* correct sql ([06731df](https://github.com/archguard/archguard/commit/06731df16a852b724b8f02e203d422ea9ecfb92e))
* correct sql ([2670bba](https://github.com/archguard/archguard/commit/2670bba94955dfbb6179d96cbf580941ac83de16))
* correct statistic ([7a8fc86](https://github.com/archguard/archguard/commit/7a8fc86c90ec7a51d6861da6cba93d1d97264653))
* correct test ([26b83cb](https://github.com/archguard/archguard/commit/26b83cb3886014042909e1c019eb37f46570e97e))
* correct test ([8f16c6b](https://github.com/archguard/archguard/commit/8f16c6b3a5827a4ed236dcc64723089a01716976))
* correct test ([6ca2353](https://github.com/archguard/archguard/commit/6ca23539c9b32dd31dc542b8534708de632b2350))
* correct test ([1220357](https://github.com/archguard/archguard/commit/1220357045d3c605879faef8b2e0511886ba6558))
* correct test ([52f9145](https://github.com/archguard/archguard/commit/52f914594a9f89ea3dcd6197126d470a2a70e417))
* correct test ([9d2c5e2](https://github.com/archguard/archguard/commit/9d2c5e2bb9383b6193c7a15ac5bfcb0c1d2ca4bf))
* correct test ([9017348](https://github.com/archguard/archguard/commit/9017348f7af5ad47d3dc539076de7154a1f552bd))
* correct test bad smell ([64e57df](https://github.com/archguard/archguard/commit/64e57df1faeae83c887af493370afa82816f7505))
* correct the package ([44d1f5e](https://github.com/archguard/archguard/commit/44d1f5ef25d66023474eb2513767166373398e8b))
* correct type ([f1446eb](https://github.com/archguard/archguard/commit/f1446ebb32a9222c0d2f2b72b7c3ba8a41dca769))
* correct url ([0aecaae](https://github.com/archguard/archguard/commit/0aecaae4b4b9ffeaa25e3e97516d008ba7f186a3))
* dubbo xml config support analysis multi-group/version ([968c1f3](https://github.com/archguard/archguard/commit/968c1f3cc0b22127347a4981401a20d838f3dd35))
* escape single quote ([839cf6b](https://github.com/archguard/archguard/commit/839cf6b9b282cf38d55a4f1d347e09255aa3af58))
* extract elementToStyle ([5eddf42](https://github.com/archguard/archguard/commit/5eddf421709c155326dd3dcf51936880ba680296))
* extract qualityReport ([ad6c1e8](https://github.com/archguard/archguard/commit/ad6c1e810d8399eadfa325ea089de6565f9962b6))
* filter unused package ([6c11d29](https://github.com/archguard/archguard/commit/6c11d29afa813399610b5dbd413f7bf93f2daba8))
* fix compile issue ([53eec6b](https://github.com/archguard/archguard/commit/53eec6bbfa9d87b4b69669cbdcfeaf1ecf413546))
* fix nullable in argumentTypes ([7125a02](https://github.com/archguard/archguard/commit/7125a02eea43d32428dbb47fbad82b5937451a20))
* fix test ([46f815b](https://github.com/archguard/archguard/commit/46f815be71c74b1dca9a2d0e275a53dfc9089c09))
* fix test ([a7a86cd](https://github.com/archguard/archguard/commit/a7a86cdfb398f7984fdadc459afb27062bdd2d71))
* fix test ([272a648](https://github.com/archguard/archguard/commit/272a648ed056e7e0db609943153f718cca50b66e))
* get coupling cached from db ([aa9d6c8](https://github.com/archguard/archguard/commit/aa9d6c853171994fba811cceb5e9a2e9fb1bbdd6))
* get FeignClient method dependencies ([a91000d](https://github.com/archguard/archguard/commit/a91000d0d8efb36865c715d0652b1fba23017431))
* get graph of logicModule ([2663751](https://github.com/archguard/archguard/commit/26637514b382259b7a38d646a589bd1836901549))
* get module coupling with inner and outer statistics ([9063854](https://github.com/archguard/archguard/commit/9063854ad9195d3c31ab19c8cfded4c315e0a886))
* get table name ([def0b37](https://github.com/archguard/archguard/commit/def0b37664048b1bca346840583d077091af520b))
* getAllClassDependency return Classes with Type ([dda20b5](https://github.com/archguard/archguard/commit/dda20b5d51f213e4f9e24aaa7701d66702358bd6))
* getLCOM4Graph ([6b00544](https://github.com/archguard/archguard/commit/6b005449c9f66ffe282eee5aff1fa6cb2b577afb))
* impl abc metric ([0928c53](https://github.com/archguard/archguard/commit/0928c531ba900e89ffcc611ed89c019458de78c1))
* impl coupling related dao ([c26021a](https://github.com/archguard/archguard/commit/c26021a0d321a25ef06ad1ae64d12f68e520b205))
* impl LogicComponent ([92c5a23](https://github.com/archguard/archguard/commit/92c5a235593e4dc2a0af40f04715e516ab16fb4f))
* implement DubboConfigRepositoryImpl ([2173003](https://github.com/archguard/archguard/commit/21730036b6c768ebc5f6822f1e9756af479def20))
* ingore bundle ([74f4b86](https://github.com/archguard/archguard/commit/74f4b86f48fa042039353b869323e6a6cc0e759d))
* init codeTree ([1e6223a](https://github.com/archguard/archguard/commit/1e6223ad93119fa9ce347ae4beb95ecb910f4c7e))
* init project ([0c3e203](https://github.com/archguard/archguard/commit/0c3e2038d090dbc3f56e6c8aed9c247dbc5d2be1))
* insertAllClassCouplings ([e608ff8](https://github.com/archguard/archguard/commit/e608ff84e08a60b4f4664eab993c027e3697540f))
* JClass add type and autodefineLogicModuleWithInteface only use interface ([6f55edb](https://github.com/archguard/archguard/commit/6f55edbce014769312571243d232111593212f40))
* JClassVo and JMethod impl Node interface ([c13177e](https://github.com/archguard/archguard/commit/c13177e394a0e5c6cd55af014a5ab4f64bc20785))
* log line ([14b2782](https://github.com/archguard/archguard/commit/14b27827f006f6f3d1a5c8c29af4ba6316674f55))
* make all scanner work ([05c8c2c](https://github.com/archguard/archguard/commit/05c8c2c519039da4ede2c39f3ad77b16df1d5460))
* make flyway and local works ([43e5d9f](https://github.com/archguard/archguard/commit/43e5d9fe76ce3bbd0f6061e69571dcbd1ae02a6b))
* make lcom4 return graph ([61bc245](https://github.com/archguard/archguard/commit/61bc2457a3b8b15af067e1dd3a92a649ec680983))
* make method related api return list of method ([b4f6abc](https://github.com/archguard/archguard/commit/b4f6abc6196d7930ba6722e7419decf6b1d8e329))
* make test work ([49ff8b2](https://github.com/archguard/archguard/commit/49ff8b2354c880ee3afa7d72ea7a7ac6ce6bf137))
* merge path to FeignClientMethod ([18e0fe6](https://github.com/archguard/archguard/commit/18e0fe69c6d8866465849202f4f1e0c5d18c84b4))
* metricService add calculate all class XXXmetric ([d3eb0ea](https://github.com/archguard/archguard/commit/d3eb0ea04ea96ae6a727037b3a338ae91ad7c44f))
* modify analysis by id ([6ed2938](https://github.com/archguard/archguard/commit/6ed29386a4b0e0d99a8a55b8c55c895a9b9adc28))
* modify auto api and plugin ([b8d5688](https://github.com/archguard/archguard/commit/b8d56884b637eb3d9aad576a84af5819a25ec070))
* modify auto define api ([b0d3a9d](https://github.com/archguard/archguard/commit/b0d3a9de4b41a8a21958628b3ebcf10df1e5ab8d))
* modify auto define post project id ([0bf1b69](https://github.com/archguard/archguard/commit/0bf1b693a3cfdb5dce31f05ceebce79188cd40e9))
* modify auto define post project id ([3663abc](https://github.com/archguard/archguard/commit/3663abcfac0f17fefc90c95643b5c3f3d972cc23))
* modify code scanner by project id ([20fcfab](https://github.com/archguard/archguard/commit/20fcfabf945843f140b3b390ef805f9f37a6a839))
* modify code scanner by project id ([b1bc6cf](https://github.com/archguard/archguard/commit/b1bc6cfd9ce49a6d4213f7e0fca0cc605a8263ca))
* modify code scanner jar version ([a810eed](https://github.com/archguard/archguard/commit/a810eed1e325c1752a4e1f6bc041d42cd0519000))
* modify code style ([9ac1d4a](https://github.com/archguard/archguard/commit/9ac1d4a610aca4938d0a06b610ec889c94ba550e))
* modify config get by project id ([e1ff575](https://github.com/archguard/archguard/commit/e1ff5757e4860fca45a3910102fe9f181d1477fa))
* modify dependency by project id ([012ac8e](https://github.com/archguard/archguard/commit/012ac8e2742776d78cc55b6b4a2c9da4bd56d5e5))
* modify dependency tables ([c820300](https://github.com/archguard/archguard/commit/c820300255f069576d38500ec80ce15918a7c635))
* modify logic module for project id ([9f97af0](https://github.com/archguard/archguard/commit/9f97af02265db429917689e2fda8c692b913a939))
* modify logic_module members can be null ([f26bb1c](https://github.com/archguard/archguard/commit/f26bb1ccea0f67d239ab54e7af44855c2973fb17))
* modify LogicModuleRepository for lgMembers ([d205efa](https://github.com/archguard/archguard/commit/d205efa8e2dda9f5d673f253799048b976b89aaa))
* modify metrics repo test ([e11bd9d](https://github.com/archguard/archguard/commit/e11bd9d0c24ea748845ce49ef8920b155bdc7630))
* modify plugin to depend plugin ([09392f7](https://github.com/archguard/archguard/commit/09392f7d59b647a09415d83b1863b61cfb583dfe))
* modify project info covert id uuid to long ([b18d18a](https://github.com/archguard/archguard/commit/b18d18a59d5c585dd7eecae8d963b8d209ae57cf))
* modify repo test transactional ([b5c4f3d](https://github.com/archguard/archguard/commit/b5c4f3df875322357cef8bb036df8481c401d454))
* modify send api to auto define ([1a0228b](https://github.com/archguard/archguard/commit/1a0228b4dc064423e471a06fee47d70405fd71ce))
* modify truncate table ([980d2d5](https://github.com/archguard/archguard/commit/980d2d5d37caeb2bcf4be60cd25bdcae08667660))
* move config to application ([f9d3f2d](https://github.com/archguard/archguard/commit/f9d3f2d674dfbadd0b0ae03c6e064b9368f77cd3))
* move download url to download ([dd284f2](https://github.com/archguard/archguard/commit/dd284f2834797d9e19c5285d7cbafd709f2b8a60))
* move get report file to check style tool ([5410b7a](https://github.com/archguard/archguard/commit/5410b7a65877cad266acf513a0e2b9d26cc51f75))
* move git , bs, tbs, git to scanner folder ([4bbf898](https://github.com/archguard/archguard/commit/4bbf898522bd801f868e2171ae9c8c5b790f8d63))
* move hub to scanner ([31e87d6](https://github.com/archguard/archguard/commit/31e87d60ac4b6afdbb9da3d6a5a2331634c6d7ae))
* move newer changes ([5aa7593](https://github.com/archguard/archguard/commit/5aa759300d59b3950e63ef3cad1e164776f2cc57))
* move repository to db ([0d28ef0](https://github.com/archguard/archguard/commit/0d28ef017c12b818a6ea1018a1263851ed3568d5))
* new Coupling metric ([af0a06e](https://github.com/archguard/archguard/commit/af0a06e18c6b1dd4326b764a36ec218ce95d126f))
* not delete ([fd11fbe](https://github.com/archguard/archguard/commit/fd11fbee2ca537df9442f13417b0924b9123e5a6))
* query all evaluatioon report ([0a188a8](https://github.com/archguard/archguard/commit/0a188a8ada789b6080d7c9324f5155541cce2ba8))
* refactor ([21441e9](https://github.com/archguard/archguard/commit/21441e95faf719520cc13d7ccea71595d6f81d20))
* refactor ([00c5788](https://github.com/archguard/archguard/commit/00c5788b29bd574b89cb54d6d78b67c1588d1454))
* refactor ([c28887c](https://github.com/archguard/archguard/commit/c28887cd651da85f97555742e21e639469a3942d))
* refactor coverage report ([89f0bec](https://github.com/archguard/archguard/commit/89f0becaa3cd9cfe696735061579770c4162ca0f))
* remove .git file ([c03b49a](https://github.com/archguard/archguard/commit/c03b49a4c99318bb64572ac81ee710b1d469895e))
* remove analysis ([7ae2697](https://github.com/archguard/archguard/commit/7ae2697f6b72b0c9aa4a57c303c53a69df09144c))
* remove cocaScanner ([839e58c](https://github.com/archguard/archguard/commit/839e58ceb7691d00dfe606207dee87c418ba1d37))
* remove dev properties ([9851cb4](https://github.com/archguard/archguard/commit/9851cb474e4799aeb077e60443e0486d520129c6))
* remove directory ([a362d9a](https://github.com/archguard/archguard/commit/a362d9ac3dcd377238366e22683a7b8032547251))
* remove git tables before add data ([8c3d6a7](https://github.com/archguard/archguard/commit/8c3d6a7df0fe25ae1ab86d8dba3f6757942c094e))
* remove logic module'members whose module is itself ([b19d8dc](https://github.com/archguard/archguard/commit/b19d8dc92e3c90e324fdb4d9a2185b2bfa339d94))
* remove report service ([97a50f3](https://github.com/archguard/archguard/commit/97a50f31e967ab0949420078fc591dbbfa108004))
* remove useless shtring buffer ([8fde929](https://github.com/archguard/archguard/commit/8fde929169f69177d44908841a262368f83725d1))
* remove useless steps ([ec6e826](https://github.com/archguard/archguard/commit/ec6e8261ffe327ce3a9c31497f8d8b35e2eac0a4))
* rename file name to style ([df90e41](https://github.com/archguard/archguard/commit/df90e41f1208a7c610ac9f2da9244930b7af6a62))
* rename JavaBytecode to JavaDependency ([c0059c4](https://github.com/archguard/archguard/commit/c0059c495447fb73b37312938892de3d6c84aeda))
* rename to tool ([bf10ab2](https://github.com/archguard/archguard/commit/bf10ab2d36eff89cbd570a6579030f646aeff436))
* replace project root dir with workspace ([645f8cd](https://github.com/archguard/archguard/commit/645f8cd27a316f0cf9dcb343d4c2f06f2cce5a14))
* return children nodes ([84ebd51](https://github.com/archguard/archguard/commit/84ebd5169b5ea55163a60b3debab8d7303979a52))
* return codetree ([f5ea775](https://github.com/archguard/archguard/commit/f5ea7759bfb0bd79ef457601b47a2cc6934a8aeb))
* return directly when request /evaluations ([b40c03a](https://github.com/archguard/archguard/commit/b40c03a05ab04c1023936095646188d286524d59))
* return jclass when getting class dependences ([7c7660a](https://github.com/archguard/archguard/commit/7c7660a53462930a7ee014a128c202770a022d4f))
* return module coupling info ([e749826](https://github.com/archguard/archguard/commit/e74982651c9924fffd2a0e282d958d59340d5db8))
* rewrite getLogicModuleCouplingByClass because getClassModule method return list ([a553b58](https://github.com/archguard/archguard/commit/a553b5840e567bb8d6babd11babbf107f30bd514))
* save class coupling to influxDB ([3a10d75](https://github.com/archguard/archguard/commit/3a10d750673ed2bf22ecce9c8c46704c24bd4798))
* sort logicmodule members ([4c839be](https://github.com/archguard/archguard/commit/4c839bed1912991448748717f97a374e6ee5a75c))
* support frontend new LogicModule which is Service ([0e67e84](https://github.com/archguard/archguard/commit/0e67e84f7417f85dcb88337d97fe9f4a05f566c4))
* support hide class when analysis method ([2749ef0](https://github.com/archguard/archguard/commit/2749ef0743312626fd9df06ac14cd14649ba8b23))
* support hide/show/reverse all logic module status ([78b02f4](https://github.com/archguard/archguard/commit/78b02f425876ae386f07b2b89d8f6076aa6dbf34))
* support Identity AbstractClass ([44444f6](https://github.com/archguard/archguard/commit/44444f63b612206fa1f8f9d293a68ad6af09f5c7))
* support ignore deploy in pipeline ([e1317be](https://github.com/archguard/archguard/commit/e1317be3e2d7fc1231204342058d71709307cdf2))
* support jmethod access to methodType transform ([92b8815](https://github.com/archguard/archguard/commit/92b8815df27b9e39fc0068caac650a18c068a749))
* support method overload ([0b93440](https://github.com/archguard/archguard/commit/0b934407803142c0a58bfd0e591ac3edfb8aae29))
* support multi-case dubbo-analysis ([534a06a](https://github.com/archguard/archguard/commit/534a06afa7c69f5a463409e79d407d77434a88e5))
* support query package direct under class coupling and packageList coupling result ([bbbb517](https://github.com/archguard/archguard/commit/bbbb5178dbd3f459e0134bb035b4f72ccf2a55c2))
* support Service in LogicModule ([115af38](https://github.com/archguard/archguard/commit/115af38ac4fad758bea4aeaceb34ffdfbd45a80c))
* support updateEdgeByAddNum ([f119f17](https://github.com/archguard/archguard/commit/f119f176be25a2350c764b33d8ea11d49324c46a))
* support use jar tools in image ([c6c255a](https://github.com/archguard/archguard/commit/c6c255aa06fb7b10020608ce8a1a2cc741ad05f9))
* update flyway script ([d33ff9c](https://github.com/archguard/archguard/commit/d33ff9cf62e8cd019ac8f64bd7b10f5f90f8f791))
* update module coupling quality ([de00481](https://github.com/archguard/archguard/commit/de0048196ef425e46d6ce3ba3b9087fb36c117cf))
* use ./mvnw and remove useless ([97368ea](https://github.com/archguard/archguard/commit/97368ead14c2ab12c485dca8a6e090fdb902369f))
* use clean compile/classes replace clean package/build ([670ddab](https://github.com/archguard/archguard/commit/670ddab65490aad0d12ab85b47a1f30161d28023))
* use configure to filter class ([07d01ec](https://github.com/archguard/archguard/commit/07d01ec424fa53f8647d27e3b0c4233302a375ba))
* use log instead of print ([02b209d](https://github.com/archguard/archguard/commit/02b209d16719ba0a33965761b6ea4c5e56a7c137))
* use map to bean ([bca8f22](https://github.com/archguard/archguard/commit/bca8f22631499740f2d699ae8de7db91c5b8953d))
* use new ci host ([8871ba7](https://github.com/archguard/archguard/commit/8871ba71ddeb09f36f3a126d048bf1885805e481))
* use parallelStream update JClass Info in getAllClassDependency ([22a5c98](https://github.com/archguard/archguard/commit/22a5c98a5ba1bf459548a3ed100750422d9863de))
* 修改Report时区默认为上海时区 ([5cc4a7b](https://github.com/archguard/archguard/commit/5cc4a7b3629182813129174d9722bdaa3081dfc0))



