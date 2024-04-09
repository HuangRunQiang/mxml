安全政策
===============

本文档描述了如何报告和处理安全问题，以及对报告给该项目的安全问题的期望。


报告安全漏洞
------------------------

对于本项目而言，安全漏洞是指允许*本地或远程用户*在主机计算机上获得未经授权的访问或特权，或导致软件崩溃的软件缺陷。应将此类缺陷报告给项目的安全咨询页面：<https://github.com/michaelrsweet/mxml/security/advisories>。

或者，也可以使用下面的PGP公钥将安全漏洞报告发送到"security AT msweet.org"。预计在5个工作日内会收到回复。任何建议的禁令日期应该至少在未来30天且不超过90天。

> *注意：*如果您发现了一个允许*程序*在主机计算机上获得未经授权的访问或特权，或导致程序崩溃的软件缺陷，请将该缺陷作为普通项目问题报告给<https://github.com/michaelrsweet/mxml/issues>。


负责任的披露
----------------------

在*负责任的披露*中，安全问题（及其修复）仅在相互约定的一段时间（"禁令日期"）之后披露。问题和修复方案会在关键利益相关者（Linux发行版、操作系统供应商等）和CERT/CC之间共享和审查。修复方案将在约定的日期向公众发布。

> 负责任的披露仅适用于生产版本。仅影响未发布代码的安全漏洞可以立即修复，无需协调。供应商*不应*打包和发布此软件的不稳定快照、测试版或预发布版本。


支持的版本
------------------

本软件的所有生产版本均适用于此安全政策。生产版本的标签和语义版本号的形式如下：

    主版本.次版本.修订版本

其中，"主版本"是从1开始的整数，"次版本"和"修订版本"是从0开始的整数。一个功能版本的"修订版本"值为0，例如：

    1.0.0
    1.1.0
    2.0.0

测试版和预发布版本不是生产版本，使用以下形式的语义版本号：

    主版本.次版本b编号
    主版本.次版本rc编号

其中，"主版本"和"次版本"标识新功能版本号，"编号"标识从1开始的测试版或预发布版号，例如：

    1.0b1
    1.0b2
    1.0rc1


PGP公钥
--------------

以下PGP公钥可用于签署安全消息。

```
-----BEGIN PGP PUBLIC KEY BLOCK-----
Comment: GPGTools - https://gpgtools.org

mQINBF6L0RgBEAC8FTqc/1Al+pWW+ULE0OB2qdbiA2NBjEm0X0WhvpjkqihS1Oih
ij3fzFxKJ+DgutQyDb4QFD8tCFL0f0rtNL1Iz8TtiAJjvlhL4kG5cdq5HYEchO10
qFeZ1DqvnHXB4pbKouEQ7Q/FqB1PG+m6y2q1ntgW+VPKm/nFUWBCmhTQicY3FOEG
q9r90enc8vhQGOX4p01KR0+izI/g+97pWgMMj5N4zHuXV/GrPhlVgo3Wn1OfEuX4
9vmv7GX4G17Me3E3LOo0c6fmPHJsrRG5oifLpvEJXVZW/RhJR3/pKMPSI5gW8Sal
lKAkNeV7aZG3U0DCiIVL6E4FrqXP4PPj1KBixtxOHqzQW8EJwuqbszNN3vp9w6jM
GvGtl8w5Qrw/BwnGC6Dmw+Qv04p9JRY2lygzZYcKuwZbLzBdC2CYy7P2shoKiymX
ARv+i+bUl6OmtDe2aYaqRkNDgJkpuVInBlMHwOyLP6fN2o7ETXQZ+0a1vQsgjmD+
Mngkc44HRnzsIJ3Ga4WwW8ggnAwUzJ/DgJFYOSbRUF/djBT4/EFoU+/kjXRqq8/d
c8HjZtz2L27njmMw68/bYmY1TliLp50PXGzJA/KeY90stwKtTI0ufwAyi9i9BaYq
cGbdq5jnfSNMDdKW2kLCNTQeUWSSytMTsdU0Av3Jrv5KQF8x5GaXcpCOTwARAQAB
tExNaWNoYWVsIFN3ZWV0IChzZWN1cml0eUBtc3dlZXQub3JnKSAoU2VjdXJpdHkg
UEdQIEtleSkgPHNlY3VyaXR5QG1zd2VldC5vcmc+iQJUBBMBCgA+FiEEOElfSXYU
h91AF0sBpZiItz2feQIFAl6L0RgCGwMFCQeGH4AFCwkIBwMFFQoJCAsFFgIDAQAC
HgECF4AACgkQpZiItz2feQIhjhAAqZHuQJkPBsAKUvJtPiyunpR6JENTUIDxnVXG
nue+Zev+B7PzQ7C4CAx7vXwuWTt/BXoyQFKRUrm+YGiBTvLYQ8fPqudDnycSaf/A
n01Ushdlhyg1wmCBGHTgt29IkEZphNj6BebRd675RTOSD5y14jrqUb+gxRNuNDa5
ZiZBlBE4A8TV6nvlCyLP5oXyTvKQRFCh4dEiL5ZvpoxnhNvJpSe1ohL8iJ9aeAd5
JdakOKi8MmidRPYC5IldXwduW7VC7dtqSiPqT5aSN0GJ8nIhSpn/ZkOEAPHAtxxa
0VgjltXwUDktu74MUUghdg2vC1df2Z+PqHLsGEqOmxoBIJYXroIqSEpO3Ma7hz0r
Xg1AWHMR/xxiLXLxgaZRvTp7AlaNjbqww8JDG8g+nDIeGsgIwWN/6uPczledvDQa
HtlMfN97i+rt6sCu13UMZHpBKOGg7eAGRhgpOwpUqmlW1b+ojRHGkmZ8oJSE7sFT
gzSGNkmfVgA1ILl0mi8OBVZ4jlUg6EgVsiPlzolH92iscK7g50PdjzpQe0m3gmcL
dpOmSL8Fti05dPfamJzIvJd28kMZ6yMnACKj9rq/VpfgYBLK8dbNUjEOQ2oq7PyR
Ye/LE1OmAJwfZQkyQNI8yAFXoRJ8u3/bRb3SPvGGWquGBDKHv2K1XiCW65uyLe5B
RNJWmme5Ag0EXovRGAEQAJZMFeIMt/ocLskrp89ZyBTTiavFKn9+QW7C2Mb36A73
J2g9vRFBSRizb+t8lSzP/T1GbKS0cEmfEpQppWImTbOMV