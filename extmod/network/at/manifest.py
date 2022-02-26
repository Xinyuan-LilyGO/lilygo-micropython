# This list of frozen files doesn't include task.py because that's provided by the C module.
freeze(
    "..",
    (
        "at/__init__.py",
        "at/gprs.py",
        "at/gps.py",
        "at/modem.py",
        "at/sim7000.py",
    ),
    opt=3,
)
