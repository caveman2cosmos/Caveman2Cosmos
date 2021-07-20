from argparse import ArgumentParser, Namespace
from enum import Enum
from pathlib import Path
from subprocess import check_call
from typing import Sequence, Tuple


class _PythonVersion(Enum):
    python_2: str = "2"
    python_3: str = "3"


def _args() -> Namespace:
    parser = ArgumentParser()
    parser.add_argument(
        "--python-versions",
        choices=tuple([version.value for version in _PythonVersion]),
        default=tuple([version.value for version in _PythonVersion]),
        help="Lints all modules targeting a specific python version, will lint both versions by default",
        nargs="+",
        required=False,
        type=str,
    )
    return parser.parse_args()


def _main() -> None:
    python_versions_to_be_linted = tuple(
        _PythonVersion(raw_version) for raw_version in _args().python_versions
    )
    for python_version in python_versions_to_be_linted:
        print(f"Attempting to lint modules for python version {python_version.value}")
        _install_pylint_for_python_version(version=python_version)
        _lint_modules_for_python_version(
            version=python_version,
            modules=_python_modules_for_python_version(python_version),
        )
        _uninstall_pylint_for_python_version(version=python_version)

    print("Linting finished")


def _python_modules_for_python_version(version: _PythonVersion) -> Tuple[Path, ...]:
    if version == _PythonVersion.python_3:
        return _python_3_modules()
    if version == _PythonVersion.python_2:
        return _python_2_modules()
    raise ValueError(f"Unsupported python version {version.value}")


def _python_2_modules():
	files = []
	for path in (_root_path() / "Assets" / "Python").rglob("*.py"):
		files.append(path)
	for path in (_root_path() / "PrivateMaps").rglob("*.py"):
		files.append(path)
	return files


def _python_3_modules() -> Tuple[Path, ...]:
    return tuple(path for path in (Path(__file__).parents[1]).resolve().rglob("*.py"))


def _root_path() -> Path:
    return (Path(__file__).parents[2]).resolve()


def _assert_valid_python_version(version: _PythonVersion) -> None:
    if version not in tuple(_PythonVersion):
        raise ValueError(f"Unsupported python version: {version.value}")


def _install_pylint_for_python_version(version: _PythonVersion) -> None:
    print(f"Installing pylint for python {version.value}")
    _assert_valid_python_version(version)
    if version == _PythonVersion.python_2:
        pylint_version = "pylint==1.9.5"
    elif version == _PythonVersion.python_3:
        pylint_version = "pylint"
    else:
        raise ValueError(f"Unsupported python version: {version.value}")
    check_call(
        args=(
            f"python{version.value}",
            "-m",
            "pip",
            "install",
            pylint_version,
        )
    )


def _uninstall_pylint_for_python_version(version: _PythonVersion) -> None:
    print(f"Uninstalling pylint for python {version.value}")
    _assert_valid_python_version(version)
    check_call(
        args=(
            f"python{version.value}",
            "-m",
            "pip",
            "uninstall",
            "--yes",
            "pylint",
        )
    )


def _lint_modules_for_python_version(
    version: _PythonVersion, modules: Sequence[Path]
) -> None:
    print(f"Linting python {version.value} modules")
    _assert_valid_python_version(version)
    config_file = (
        _root_path() / "Tools" / "Lint" / f".pylintrc_for_python_{version.value}"
    )

    check_call(
        args=(
            f"python{version.value}",
            "-m",
            "pylint",
            "--jobs=0",
            f"--rcfile={config_file}",
            *[path.resolve() for path in modules],
        )
    )


if __name__ == "__main__":
    _main()
