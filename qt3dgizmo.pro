TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = qt3dgizmo tests

tests.depends = qt3dgizmo
