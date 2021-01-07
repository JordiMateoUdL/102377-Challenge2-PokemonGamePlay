FROM ubuntu:latest

RUN apt-get update && \
    apt-get -y install gcc mono-mcs build-essential && \
    rm -rf /var/lib/apt/lists/*

RUN mkdir /root/tmp

ENV USER pokemon-user

COPY . /home/$USER/

WORKDIR /home/$USER/

CMD [ "make" "./your-daemon-or-script.sh"]