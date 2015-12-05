
As the *pecl* command does not support distributing several extension source trees in a single package, examples of PHP extensions using the PCS service are now located in separate repositories :

- An example of a basic client extension [is available here](https://github.com/flaupretre/pcs-example). This may be used as a model for your own extension.
- The purpose of the [PCS testing extension](https://github.com/flaupretre/pcs-test) is to test every PCS features. We need to use a separate client extension because of the nature of PCS (a service for other extensions). In order to run the full PCS test suite, compile the [PCS testing extension](https://github.com/flaupretre/pcs-test) and run 'make test'.

These extensions require the PCS extension to be installed and activated (check with 'php -m | grep pcs') before they can be configured and compiled.
