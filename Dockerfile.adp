FROM fedora:26
RUN dnf install -y libserialport libserialport-devel gcc make iperf3 gcc-c++ cmake libtool libtirpc sqlite sqlite-devel sigar sigar-devel iputils libuuid-devel redhat-rpm-config python3 python3-devel lapack-devel python3-scipy python3-matplotlib freetype-devel libjpeg-turbo-devel libcurl-devel libsigc++20 gtkmm30 gtkmm30-devel clips clips-devel clipsmm clipsmm-devel
RUN dnf install -y lcov valgrind procps
ADD scripts /compile/scripts
WORKDIR /compile
RUN cat scripts/requirements.txt | xargs -n 1 -L 1 pip3 install
RUN cp -R ./scripts /

ADD assolo-0.9a /compile/assolo-0.9a
WORKDIR /compile/assolo-0.9a
RUN ./configure --build=unknown-unknown-linux
RUN make
RUN cp $(ls ./Bin/*/*) /

ADD src/follower/rules-exp1-dm.clp /clips/rules-exp1-dm.clp
ADD src/follower/rules-exp1-ctr.clp /clips/rules-exp1-ctr.clp
ADD src/follower/rules-exp1-dm-ctr.clp /clips/rules-exp1-dm-ctr.clp
ADD src/follower/rules-exp2.clp /clips/rules-exp2.clp
ADD src/leader/leader_rules.clp /clips/leader_rules.clp

ADD . /compile
WORKDIR /compile

#RUN cmake . -DCMAKE_BUILD_TYPE=Debug
RUN cmake .
RUN make
RUN cp ./FogMon /
RUN cp ./libsqlitefunctions.so /
WORKDIR /

RUN rm -Rf /compile

RUN dnf install -y iproute
ENTRYPOINT []
CMD []