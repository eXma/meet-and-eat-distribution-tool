FROM debian:jessie

MAINTAINER Jan Losinski <losinski@wh2.tu-dresden.de>

RUN DEBIAN_FRONTEND=noninteractive DEBIAN_NONINTERACTIVE_SEEN=true && \
	apt-get update && \
	apt-get -y install --no-install-recommends \
		build-essential \
		cmake \
		g++-4.9 \
		libboost-python-dev \
		python3.4-dev \
		libboost-dev \
		libtcmalloc-minimal4 \
		libgoogle-perftools4 \
		cxxtest \
	&& \
	apt-get clean && \
	rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

RUN groupadd user && \
	useradd --uid 1000 --create-home --home-dir /home/user -g user user

RUN mkdir /plandata && \
	chown user:user /plandata

WORKDIR /home/user
USER user

ADD . /home/user/src
RUN mkdir build && \
	mkdir run && \
	cd build && \
	cmake -DCMAKE_INSTALL_PREFIX:PATH=/home/user/run -DCMAKE_BUILD_TYPE=Release ../src && \
	make all install

ADD scripts/run_container.sh /home/user/run_container.sh

VOLUME ["/plandata"]

ENTRYPOINT ["/home/user/run_container.sh"]
