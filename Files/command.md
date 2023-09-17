

# 记录一些操作的命令

- [记录一些操作的命令](#记录一些操作的命令)
  - [一、记录生成PGP密钥并拉取Github库](#一记录生成pgp密钥并拉取github库)
  - [二、降低ubuntu虚拟机硬盘占用空间大小](#二降低ubuntu虚拟机硬盘占用空间大小)

## 一、记录生成PGP密钥并拉取Github库

```bash

# 1. 生成pgp密钥
gpg --full-generate-key

# 2. 按提示完成设置

# 3. 查看公钥和私钥的长形式 GPG 密钥
gpg --list-secret-keys --keyid-format=long

# ------------------------------------
# sec   4096R/3AA5C34371567BD2 2016-03-10 [expires: 2017-03-10]
# uid                          Hubot <hubot@example.com>
# ssb   4096R/4BB6D45482678BE3 2016-03-10

# 4. 输出要使用的GPG密钥的公钥(下面GPG密钥改为自己的密钥)
gpg --armor --export 3AA5C34371567BD2

# 5. 复制以 -----BEGIN PGP PUBLIC KEY BLOCK----- 开头并以 -----END PGP PUBLIC KEY BLOCK----- 结尾的 GPG 密钥。

# 6. 将 GPG 密钥新增到 GitHub 帐户，参考：https://docs.github.com/zh/authentication/managing-commit-signature-verification/adding-a-gpg-key-to-your-github-account

# 7. 如果之前已将 Git 配置为在使用 --gpg-sign 签名时使用不同的密钥格式，请取消设置此配置，以便使用默认 openpgp 格式。
git config --global --unset gpg.format

# 8. 在 Git 中设置 GPG 签名主键，请粘贴下面的文本，替换要使用的 GPG 主键 ID。 在本例中，GPG 密钥 ID 为 3AA5C34371567BD2：
git config --global user.signingkey 3AA5C34371567BD2

# 9. 若要将 Git 配置为默认对所有提交进行签名，请输入以下命令：
git config --global commit.gpgsign true

# 10. 若要将 GPG 密钥添加到 .bashrc 启动文件，请运行以下命令：
[ -f ~/.bashrc ] && echo -e '\nexport GPG_TTY=$(tty)' >> ~/.bashrc

# 11. 全局配置用户名和邮箱，将XXXX换为用户名和邮箱
git config --global user.name "XXXX"
git config --global user.email "XXXX"

# 12. 克隆Github仓库
git clone https://github.com/BingqiangZhou/LearningSLAM_ROS.git

```

参考链接：
- [在Github上使用GPG的全过程](https://zhuanlan.zhihu.com/p/76861431)
- [生成新 GPG 密钥](https://docs.github.com/zh/authentication/managing-commit-signature-verification/generating-a-new-gpg-key)
- [将您的签名密钥告知 Git](https://docs.github.com/zh/authentication/managing-commit-signature-verification/telling-git-about-your-signing-key)

## 二、降低ubuntu虚拟机硬盘占用空间大小

```bash
# 1. 虚拟机下执行如下命令，查看disk挂载目录
sudo vmware-toolbox-cmd disk list

# 2. 缩小挂载点内存：这里需要确保主机有足够的空间，一般是虚拟机大小的一倍空间。
sudo vmware-toolbox-cmd disk shrink /

# 3. 等待100%后将会进入压缩，新版本（15.5.6）的会压缩完成后自动重启，在虚拟机设置中查看发现磁盘占用空间就会减小

# 4. 如果占用空间没有减少，需要关闭虚拟机后，选中虚拟机进入设置，然后选择硬盘，然后点击压缩等待完成即可。

```


参考链接：
- [VMware ubuntu压缩虚拟磁盘](https://www.jianshu.com/p/e3695489a958)
- [如何降低ubuntu虚拟机硬盘占用空间大小](https://blog.csdn.net/fb362203/article/details/115971463)
- [windows-soft3：VMware 下减小ubuntu系统占用系统盘大小](https://blog.csdn.net/qq_34160841/article/details/106038663)