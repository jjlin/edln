#!/bin/bash

if [[ $# -ne 2 ]]; then
    echo "usage: $0 <distro> <docker-tag>"
    exit 1
fi

distro="$1"
tag="$2"
arch='x86_64'
build_dir='/build'
container_name='edln'

function docker-exec() {
    docker exec -t "${container_name}" "$@"
}

# Delete the container from any previous run.
docker rm -f "${container_name}" 2>/dev/null

# Start a new container, mounting in the source repo.
docker run -td --name "${container_name}" \
       --volume "$(pwd)":${build_dir} \
       --workdir ${build_dir} \
       "${distro}":"${tag}" \
       sleep infinity

# Install distro-specific dependencies needed for the build.
case "${distro}" in
    centos|rhel)
        deps=(gcc ncurses-devel readline-devel make)
        docker-exec yum makecache
        docker-exec yum -y install "${deps[@]}"
        ;;
    debian|ubuntu)
        deps=(gcc libncurses5-dev libreadline-dev make)
        docker-exec apt-get update
        docker-exec apt-get install -qy "${deps[@]}"
        ;;
esac

# Do the actual build.
docker-exec make

# Save off the build artifact.
mv edln edln-${distro}-${tag}-${arch}

# To facilitate debugging, leave the Docker container running.
