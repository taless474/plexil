# Makefile for Plexil compiler test suite

SMALL_TESTS = empty0 empty1 empty2 empty3 empty4 empty5 empty6 \
              command0 command1 command2 command3 command4 ComputedCommandName \
              sequence0 sequence1 sequence2 sequence3 \
              assignment0 assignment1 assignment2 \
              comment1 concurrence0 concurrence1 concurrence1 concurrence2 concurrence3 \
              Fibs Fib PrintFib FibTest LibraryCall1 LibraryCall2 LibraryCall3 \
              array-copy CopyReals on-command1 on-command2 isKnown1 for1 for2 \
              on-message1 on-message2 concat1 nodeState1 nodeState2 \
              nodeOutcome1 command-handle1 interface-test interface-test1 \
              DriveToSchool SafeDrive CruiseControl sync-cmd-test sync-cmd-test1 \
              simpleIf simpleIfThenElse simpleAssignment while1 AddArray \
              Decrement Increment Sum RoboSimSimple2 SimpleDrive \
              array-add array-complex array-copy  basic-wait-test boolean1 \
              duplicate-node-name ifWithClauseIds DriveToTarget Exchange \
              library-call1 library-node1 LibraryCallWithArray literal-format-test \
              if2 if3 test-recv test-send \
              lookup2 LookupWithComputedStateName \
              maxTest minTest mod1 expr1 expr3 expr4 \
              try1 update1 update2 update3 \
              resource0 Resource1 Resource2 Resource3 Resource4 \
			  StringArrayLiteral tolerance1 \
              invariantFail1 succeeded1 waiting1 executing1 finished1 failed1 skipped1 \
              iterationSucceeded1 iterationEnded1 iterationFailed1 preconditionFailed1 \
	      inactive1 wait1 date1 time-lookup-test time-lookup-test1 timed-iteration

SYNTAX_ERROR_TESTS   = array-library-call

SEMANTIC_ERROR_TESTS = on-message3 expr5 expr2 lookup1

TESTS	         := $(SMALL_TESTS)

# The VALID_TEST* macros are used by the original test framework.  

VALID_TESTS	:= $(TESTS)

VALID_TEST_FILES := $(VALID_TESTS:%=input/%.ple)

default: test

all: setup valid-tests syntax-error-tests semantic-error-tests xml-validate

setup:
	-$(RM) tempRegressionResults

valid-tests: $(VALID_TEST_FILES) output | setup
	@ failure=0 ; \
	touch tempRegressionResults ; \
	echo '--- Begin valid-file tests ---' >> tempRegressionResults ; \
	for f in $(VALID_TEST_FILES) ; do \
		./validate $$f 2>> tempRegressionResults ; \
		if [ $$? -ne 0 ] ; then failure=1 ; echo $$f: validation failed ; fi; \
	done ; \
	exit $${failure}

# Temporary file
DIFF = /tmp/plexil-$$$$

test: output
	@ echo Translating Standard Plexil tests...
	@ $(RM) -f output/*.epx
	@ for i in $(TESTS); do \
	        ../PlexilCompiler --epx-only -o output/$$i.epx input/$$i.ple ; \
		diff output/$$i.epx valid/valid-$$i.epx \
		  > $(DIFF) ; \
		diffsize=`wc $(DIFF) | awk '{print $$1}'` ; \
		if [ $$diffsize -eq 0 ] ; then \
		  echo "     Verified $$i.epx" ; \
		else \
		  echo WARNING: $$i.epx does not verify! ; \
		fi ; \
	  done
	@ echo Finished.

# For developers: Check the current output agains the benchmarks.
checkall:
	@ for i in $(TESTS); do \
		./check $$i ; \
	  done

# For developers: Bless the current output, making it the new benchmark.
passall:
	@ for i in $(TESTS); do \
		./pass $$i ; \
	  done

syntax-error-tests: output
	@ syntactic_failure=0; \
	touch tempRegressionResults ; \
	echo '--- Begin syntax error tests ---' >> tempRegressionResults ; \
	for f in $(SYNTAX_ERROR_TESTS) ; do \
		echo $$f >> tempRegressionResults ; \
		../PlexilCompiler --epx-only -o output/$$f.epx input/$$f.ple 1>> tempRegressionResults 2>&1 ; \
		if [ $$? -eq 0 ] ; then syntactic_failure=1 ; echo $$f.ple: failed to detect expected syntax error ; fi ; \
	done ; \
	exit $${syntactic_failure}

semantic-error-tests: output
	@ semantic_failure=0; \
	touch tempRegressionResults ; \
	echo '--- Begin semantic error tests ---' >> tempRegressionResults ; \
	for f in $(SEMANTIC_ERROR_TESTS) ; do \
		echo $$f >> tempRegressionResults ; \
		../PlexilCompiler --epx-only -o output/$$f.epx input/$$f.ple 1>> tempRegressionResults 2>&1 ; \
		if [ $$? -eq 0 ] ; then semantic_failure=1 ; echo $$f.ple: failed to detect expected semantic error ; fi ; \
	done ; \
	exit $${semantic_failure}

xml-validate: output
	@ validation_failure=0 ; \
	for f in $(TESTS) ; do \
		input=input/$$f.ple ; output=output/$$f.epx ; \
		../PlexilCompiler --epx-only -o $${output} $${input} > /dev/null 2>&1 ; \
		if [ -r $${output} ] ; then \
			xmllint --noout --schema ../../../../schema/extended-plexil.xsd $${output} > /dev/null 2>&1 ; \
			if [ $$? -ne 0 ] ; then validation_failure=1 ; echo $${output}: validation failed ; fi ; \
		else \
			echo $${input}: compilation failed ; \
			validation_failure=1 ; \
		fi ; \
	done ; \
	exit $${validation_failure}

output:
	-mkdir -p output

clean:
#	-$(RM) -r output
	-$(RM) tempRegressionResults input/*.epx input/*.plx output/*.epx
