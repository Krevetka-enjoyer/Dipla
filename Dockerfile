FROM ardupilot/ardupilot-dev-clang

WORKDIR /app/third_party

RUN apt-get update && \
    apt-get install -y \
      libboost-all-dev \
      libasio-dev \
      libpq-dev \
      libgtest-dev \
      libcurl4-openssl-dev \
      postgresql-server-dev-all \
      cmake \
      git
     
RUN git clone https://github.com/CrowCpp/Crow.git && \
      cd Crow && \
      mkdir build && cd build && \
      cmake .. -DCROW_BUILD_EXAMPLES=OFF -DCROW_BUILD_TESTS=OFF && \
      make install && cd ../../ && \
    git clone https://github.com/nlohmann/json
    
RUN git clone https://github.com/jtv/libpqxx.git && \
      cd libpqxx && \
      mkdir build && cd build && \
      cmake .. && \
      cmake --build . && \
      cmake --install .
      
RUN git clone https://github.com/arun11299/cpp-jwt.git
   
COPY CMakeLists.txt /app/
COPY src /app/src/

WORKDIR /app/build

RUN cmake .. && cmake --build .


    
