FROM gcc:12.2

RUN apt-get update
RUN apt-get install libaio-dev -y
RUN apt-get install liburing-dev -y
RUN apt-get install p7zip-full -y
RUN apt-get install python3-pip -y
RUN apt-get install cmake -y
RUN pip install conan
COPY ./install_xmake.sh /
RUN /install_xmake.sh

COPY ./entrypoint.sh /
WORKDIR /source
COPY ./server.conf /root/.xmake/service/server.conf



CMD [ "/entrypoint.sh" ]