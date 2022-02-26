# This list of frozen files doesn't include task.py because that's provided by the C module.
freeze(
    ".",
    (
        "__init__.py",
        "gprs.py",
        "gps.py",
        "modem.py",
        "sim7000.py",
    ),
    opt=3,
)
