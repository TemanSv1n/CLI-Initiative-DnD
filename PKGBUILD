pkgname=cliniti
pkgver=1.0.0
pkgrel=1

pkgdesc="Simple terminal initiative tracker for DnD"
arch=('x86_64')
url="https://github.com/TemanSv1n/CLI-Initiative-DnD"
license=('GPL2')

depends=('ncurses')
makedepends=('cmake')

source=("$pkgname-$pkgver.tar.gz")
sha256sums=('SKIP')

build() {
    cmake -B build \
          -S . \
          -DCMAKE_BUILD_TYPE=Release \
          -DCMAKE_INSTALL_PREFIX=/usr

    cmake --build build
}

package() {
    DESTDIR="$pkgdir" cmake --install build
}
