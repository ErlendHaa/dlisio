from .basicobject import BasicObject
from .valuetypes import scalar, vector
from .utils import describe_dict

from collections import OrderedDict

class Origin(BasicObject):
    """ Describes the creation of the logical file.

    Origin objects is an unique indentifier for a Logical File and it
    describes the circumstances under which the file was created. The Origin
    object also spesify the Logical File's relation to a DLIS-file and to
    which Logical Set it belongs.

    A logical file may have several Origin objects, whereas the first Origin
    object is the Defining object. No two logical files should have identical
    Defining Origins.

    Attributes
    ----------

    file_id : str
        An exact copy of Fileheader.id

    file_set_name : str
        The name of the File Set that the Logical File is a part of

    file_set_nr : int
        The number of the File Set that the Logical File is a part of

    file_nr : int
        The file number of the Logical File within a File Set

    file_type : str
        A producer spesified File-Type that signifies the content of the
        DLIS-file

    product : str
        Name of the software product that produced the DLIS-file

    version : str
        The version of the software product that created the DLIS-file

    programs : list(str)
        Other programs and services that was a part of the software that
        created the DLIS-file

    creation_time : datetime
        Date and time at which the DLIS-File was created

    order_nr : str
        An unique accounting number assosiated with the creation of the
        DLIS-File

    descent_nr
        The meaning of this number must be obtained directly from the producer

    run_nr
        The meaning of this number must be obtained directly from the company

    well_id
        Id of the well at which the measurements where acquired

    well_name : str
        Name of the well at which the measurements where acquired

    field_name : str
        The field to which the well belongs

    producer_code : int
        The producer's identifying code

    producer_name : str
        The producer's name

    company : str
        The name of the client company which the log was produced for

    namespace_name : str
        (DLIS internal) A producer-defined namespace for which the object names
        for this origin are defined under

    namespace_version : int
        (DLIS internal) The version of the namespace.

    See also
    --------

    BasicObject : The basic object that Origin is derived from
    Fileheader : Fileheader

    Notes
    -----

    The Origin object reflects the logical record type ORIGIN, defined in rp66.
    ORIGIN records are listed in Appendix A.2 - Logical Record Types and
    described in detail in Chapter 5.1 - Static and Frame Data, Origin objects.

    """
    attributes = {
        'FILE-ID'           : scalar,
        'FILE-SET-NAME'     : scalar,
        'FILE-SET-NUMBER'   : scalar,
        'FILE-NUMBER'       : scalar,
        'FILE-TYPE'         : scalar,
        'PRODUCT'           : scalar,
        'VERSION'           : scalar,
        'PROGRAMS'          : vector,
        'CREATION-TIME'     : scalar,
        'ORDER-NUMBER'      : scalar,
        'DESCENT-NUMBER'    : vector,
        'RUN-NUMBER'        : vector,
        'WELL-ID'           : scalar,
        'WELL-NAME'         : scalar,
        'FIELD-NAME'        : scalar,
        'PRODUCER-CODE'     : scalar,
        'PRODUCER-NAME'     : scalar,
        'COMPANY'           : scalar,
        'NAME-SPACE-NAME'   : scalar,
        'NAME-SPACE-VERSION': scalar,
    }

    def __init__(self, obj = None, name = None, lf=None):
        super().__init__(obj, name = name, type = 'ORIGIN', lf=lf)

    @property
    def file_id(self):
        return self['FILE-ID']

    @property
    def file_set_name(self):
        return self['FILE-SET-NAME']

    @property
    def file_set_nr(self):
        return self['FILE-SET-NUMBER']

    @property
    def file_nr(self):
        return self['FILE-NUMBER']

    @property
    def file_type(self):
        return self['FILE-TYPE']

    @property
    def product(self):
        return self['PRODUCT']

    @property
    def version(self):
        return self['VERSION']

    @property
    def programs(self):
        return self['PROGRAMS']

    @property
    def creation_time(self):
        return self['CREATION-TIME']

    @property
    def order_nr(self):
        return self['ORDER-NUMBER']

    @property
    def descent_nr(self):
        return self['DESCENT-NUMBER']

    @property
    def run_nr(self):
        return self['RUN-NUMBER']

    @property
    def well_id(self):
        return self['WELL-ID']

    @property
    def well_name(self):
        return self['WELL-NAME']

    @property
    def field_name(self):
        return self['FIELD-NAME']

    @property
    def producer_code(self):
        return self['PRODUCER-CODE']

    @property
    def producer_name(self):
        return self['PRODUCER-NAME']

    @property
    def company(self):
        return self['COMPANY']

    @property
    def namespace_name(self):
        return self['NAME-SPACE-NAME']

    @property
    def namespace_version(self):
        return self['NAME-SPACE-VERSION']

    def describe_attr(self, buf, width, indent, exclude):
        fileset  = '{} / {}'.format(self.file_set_name, self.file_set_nr)
        fileinfo = '{} / {}'.format(self.file_nr, self.file_type)
        d = OrderedDict()
        d['Logical file ID']          = self.file_id
        d['File set name and number'] = fileset
        d['File number and type']     = fileinfo

        describe_dict(buf, d, width, indent, exclude)

        well     = '{} / {}'.format(self.well_id, self.well_name)
        producer = '{} / {}'.format(self.producer_code, self.producer_name)
        d = OrderedDict()
        d['Field']                   = self.field_name
        d['Well (id/name)']          = well
        d['Produced by (code/name)'] = producer
        d['Produced for']            = self.company
        d['Order number']            = self.order_nr
        d['Run number']              = self.run_nr
        d['Descent number']          = self.descent_nr
        d['Created']                 = self.creation_time

        describe_dict(buf, d, width, indent, exclude)

        prog = '{}, (version: {})'.format(self.product, self.version)
        d = OrderedDict()
        d['Created by'] = prog
        d['Other programs/services'] = self.programs

        describe_dict(buf, d, width, indent, exclude)
